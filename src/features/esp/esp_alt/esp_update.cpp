#include "../esp_main/esp_internal.h"
#include "../../../core/dll_main/globals.h"
#include <Windows.h>

namespace ESP {
    void updateESPInt() {
        std::uintptr_t localPlayerPtr = 0;
        auto localPlayer = readLocPlayer(localPlayerPtr);
        if (!localPlayerPtr || localPlayer.formId == 0x00000014) {
            g_players.clear();
            g_npcs.clear();
            g_enemies.clear();
            g_entities.clear();
            return;
        }
        Vec3 localPlayerWorldPos{};
        float worldX = 0.0f, worldY = 0.0f, worldZ = 0.0f;
        if (getWorldCoords(worldX, worldY, worldZ)) {
            localPlayerWorldPos = Vec3{ worldX, worldY, worldZ };
        } else {
            localPlayerWorldPos = localPlayer.position;
        }
        Vec3 cellOriginOffset = {
            localPlayerWorldPos.x - localPlayer.position.x,
            localPlayerWorldPos.y - localPlayer.position.y,
            localPlayerWorldPos.z - localPlayer.position.z
        };
        float offsetMagnitude = std::sqrt(cellOriginOffset.x * cellOriginOffset.x +
            cellOriginOffset.y * cellOriginOffset.y +
            cellOriginOffset.z * cellOriginOffset.z);
        if (offsetMagnitude < 1.0f)
            cellOriginOffset = Vec3{ 0.0f, 0.0f, 0.0f };

        g_players.clear();
        g_npcs.clear();
        g_enemies.clear();
        g_entities.clear();
        float screenW = 1920.0f;
        float screenH = 1080.0f;
        getScreenSize(screenW, screenH);

        auto cells = getLoadedCells();
        for (const auto& cell : cells) {
            auto objects = getCellObj(cell);
            for (const auto& [objPtr, refr] : objects) {
                if (refr.formId == localPlayer.formId) continue;
                TESItem baseObj{};
                if (!Rpm(refr.baseObjectPtr, &baseObj, sizeof baseObj)) continue;
                bool isPlayer = (baseObj.formId == 0x00000007);
                if (refr.position.x == 0.0f && refr.position.y == 0.0f && refr.position.z == 0.0f) continue;

                auto category = Classify(baseObj);
                auto refFormType = refr.GetFormType();
                auto baseFormType = baseObj.GetFormType();

                if (category == EntityCategory::Other || category == EntityCategory::Invalid) {
                    if (!isPlayer && (refFormType == FormType::TESActor || refFormType == FormType::TESNPC))
                        category = EntityCategory::Npc;
                    else if (refFormType == FormType::TESFlora) category = EntityCategory::Flora;
                    else if (refFormType == FormType::TESObjectWEAP) category = EntityCategory::Weapon;
                    else if (refFormType == FormType::TESObjectARMO) category = EntityCategory::Armor;
                    else if (refFormType == FormType::TESAmmo) category = EntityCategory::Ammo;
                    else if (refFormType == FormType::AlchemyItem || refFormType == FormType::TESUtilityItem) category = EntityCategory::Aid;
                    else if (refFormType == FormType::TESObjectCONT) category = EntityCategory::Container;
                    else if (refFormType == FormType::TESObjectMISC) category = EntityCategory::Misc;
                    else if (refFormType == FormType::TESObjectACTI || refFormType == FormType::TESObjectDOOR ||
                        refFormType == FormType::BGSHazard || refFormType == FormType::TESObjectSTAT || refFormType == FormType::TESObjectLIGH)
                        category = EntityCategory::Invalid;
                }
                if (!isPlayer && (refFormType == FormType::TESActor || baseFormType == FormType::TESNPC)) category = EntityCategory::Npc;
                else if (refFormType == FormType::TESFlora || baseFormType == FormType::TESFlora) category = EntityCategory::Flora;
                else if (refFormType == FormType::TESObjectWEAP || baseFormType == FormType::TESObjectWEAP) category = EntityCategory::Weapon;
                else if (refFormType == FormType::TESObjectARMO || baseFormType == FormType::TESObjectARMO) category = EntityCategory::Armor;
                else if (refFormType == FormType::TESAmmo || baseFormType == FormType::TESAmmo) category = EntityCategory::Ammo;
                else if (refFormType == FormType::AlchemyItem || baseFormType == FormType::AlchemyItem ||
                    refFormType == FormType::TESUtilityItem || baseFormType == FormType::TESUtilityItem) category = EntityCategory::Aid;
                else if (refFormType == FormType::TESObjectCONT || baseFormType == FormType::TESObjectCONT) category = EntityCategory::Container;

                if (category == EntityCategory::Invalid && !isPlayer) continue;
                Vec3 worldPos = {
                    refr.position.x + cellOriginOffset.x,
                    refr.position.y + cellOriginOffset.y,
                    refr.position.z + cellOriginOffset.z
                };
                float dist = refr.position.DistanceTo(localPlayer.position);
                int distM = static_cast<int>(dist * 0.01f);
                if (static_cast<float>(distM) > Config::maxDistance) continue;

                EntityData ent;
                ent.ptr = objPtr;
                ent.formId = refr.formId;
                ent.position = worldPos;
                ent.distance = dist;
                ent.distanceM = distM;
                ent.isNPC = (category == EntityCategory::Npc);
                ent.isPlayer = isPlayer;
                ent.category = category;
                ent.actorState = getAS(refr);
                ent.isEnemy = (category == EntityCategory::Npc && !isPlayer && ent.actorState == ActorState::Alive);
                readAS(refr, ent);
                ent.name = getBaseObjName(baseObj);

                if (!ent.name.empty()) {
                    size_t p = ent.name.find('[');
                    if (p != std::string::npos) ent.name = ent.name.substr(0, p);
                    p = ent.name.find(" [");
                    if (p != std::string::npos) ent.name = ent.name.substr(0, p);
                    while (!ent.name.empty() && ent.name.back() == ' ') ent.name.pop_back();
                }

                ent.screenPos = W2S(worldPos, screenW, screenH, ent.onScreen);

                if (isPlayer) {
                    g_players.push_back(std::move(ent));
                } else if (category == EntityCategory::Npc) {
                    g_npcs.push_back(std::move(ent));
                    if (ent.isEnemy) g_enemies.push_back(ent);
                } else {
                    bool shouldSkip = false;
                    if (category == EntityCategory::Weapon) { if (refFormType != FormType::TESObjectWEAP && baseFormType != FormType::TESObjectWEAP) shouldSkip = true; }
                    else if (category == EntityCategory::Armor) { if (refFormType != FormType::TESObjectARMO && baseFormType != FormType::TESObjectARMO) shouldSkip = true; }
                    else if (category == EntityCategory::Flora) { if (refFormType != FormType::TESFlora && baseFormType != FormType::TESFlora) shouldSkip = true; }
                    else if (category == EntityCategory::Aid) { if (refFormType != FormType::AlchemyItem && baseFormType != FormType::AlchemyItem && refFormType != FormType::TESUtilityItem && baseFormType != FormType::TESUtilityItem) shouldSkip = true; }
                    if (shouldSkip) continue;
                    g_entities.push_back(std::move(ent));
                }
            }
        }
        auto base = GetModuleBase();
        std::uintptr_t falloutMainPtr = 0;
        if (Rpm(base + esp_main, &falloutMainPtr, sizeof falloutMainPtr) && IsValidPtr(falloutMainPtr)) {
            FalloutMain falloutMain{};
            if (Rpm(falloutMainPtr, &falloutMain, sizeof falloutMain) && IsValidPtr(falloutMain.platformSessionMgr)) {
                PlatformSessionManager sessionMgr{};
                if (Rpm(falloutMain.platformSessionMgr, &sessionMgr, sizeof sessionMgr) && IsValidPtr(sessionMgr.clientAccountMgr)) {
                    ClientAccountManager accountMgr{};
                    if (Rpm(sessionMgr.clientAccountMgr, &accountMgr, sizeof accountMgr) && IsValidPtr(accountMgr.clientAccountArray)) {
                        std::size_t totalAccounts = static_cast<std::size_t>(accountMgr.arraySizeA) + accountMgr.arraySizeB;
                        if (totalAccounts > 0 && totalAccounts < 1000) {
                            auto accountPtrs = std::make_unique<std::uintptr_t[]>(totalAccounts);
                            if (Rpm(accountMgr.clientAccountArray, accountPtrs.get(), totalAccounts * sizeof(std::uintptr_t))) {
                                for (std::size_t i = 0; i < totalAccounts; i++) {
                                    if (!IsValidPtr(accountPtrs[i])) continue;
                                    ClientAccountBuffer accBuf{};
                                    if (!Rpm(accountPtrs[i], &accBuf, sizeof accBuf)) continue;
                                    if (!IsValidPtr(accBuf.clientAccountPtr)) continue;
                                    ClientAccount account{};
                                    if (!Rpm(accBuf.clientAccountPtr, &account, sizeof account)) continue;
                                    if (account.formId == 0xFFFFFFFF || account.formId == localPlayer.formId) continue;
                                    auto entityPtr = GetPtrFromFormId(account.formId);
                                    if (!IsValidPtr(entityPtr)) continue;
                                    TESObjectRefr playerRefr{};
                                    if (!Rpm(entityPtr, &playerRefr, sizeof playerRefr)) continue;
                                    TESItem playerBase{};
                                    if (!Rpm(playerRefr.baseObjectPtr, &playerBase, sizeof playerBase)) continue;
                                    if (playerBase.formId != 0x00000007) continue;
                                    if (playerRefr.position.x == 0.0f && playerRefr.position.y == 0.0f && playerRefr.position.z == 0.0f) continue;

                                    Vec3 playerWorldPos = {
                                        playerRefr.position.x + cellOriginOffset.x,
                                        playerRefr.position.y + cellOriginOffset.y,
                                        playerRefr.position.z + cellOriginOffset.z
                                    };
                                    EntityData pEnt;
                                    pEnt.ptr = entityPtr;
                                    pEnt.formId = playerRefr.formId;
                                    pEnt.isPlayer = true;
                                    pEnt.name = readPlayerName(account);
                                    if (!pEnt.name.empty()) {
                                        size_t p = pEnt.name.find('[');
                                        if (p != std::string::npos) pEnt.name = pEnt.name.substr(0, p);
                                        p = pEnt.name.find(" [");
                                        if (p != std::string::npos) pEnt.name = pEnt.name.substr(0, p);
                                        while (!pEnt.name.empty() && pEnt.name.back() == ' ') pEnt.name.pop_back();
                                    }
                                    pEnt.position = playerWorldPos;
                                    pEnt.distance = playerRefr.position.DistanceTo(localPlayer.position);
                                    pEnt.distanceM = static_cast<int>(pEnt.distance * 0.01f);
                                    pEnt.screenPos = W2S(playerWorldPos, screenW, screenH, pEnt.onScreen);
                                    pEnt.actorState = getAS(playerRefr);
                                    readAS(playerRefr, pEnt);
                                    g_players.push_back(std::move(pEnt));
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void UpdateESP() {
        if (!Config::enable) {
            g_players.clear();
            g_npcs.clear();
            g_enemies.clear();
            g_entities.clear();
            return;
        }
        __try {
            updateESPInt();
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            g_players.clear();
            g_npcs.clear();
            g_enemies.clear();
            g_entities.clear();
        }
    }
}