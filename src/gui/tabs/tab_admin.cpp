#include "../menu_main/gui.h"
#include "../../core/dll_main/globals.h"
#include "../../../libs/imgui/imgui.h"
#include "../../features/scripts_lader_date/call_function.h"
#include "../../features/esp/esp_main/esp_internal.h"
#include <cstdio>

static void AdminSectionHeader(const char* label) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    ImVec2 pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(pos.x + 1, pos.y + 1));
    ImGui::Text("%s", label);
    ImGui::SetCursorPos(pos);
    ImGui::Text("%s", label);
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();
}

void renderAdminTab() {
    ImGui::Spacing();
    AdminSectionHeader("SCRIPT CALLS");

    static char cSpellFormid[16]         = "";
    static char cItemFormid[16]          = "";
    static char cCellFormid[16]          = "";
    static char cDispelSpellFormid[16]   = "";
    static char cPerkFormid[16]          = "";
    static char cAddSpellFormid[16]      = "";
    static char cIdleFormid[16]          = "";
    static char cIdleName[64]            = "";
    static char cAVName[32]              = "Health";
    static char cAVValueBuf[32]          = "100.0";
    static char cXPBuf[32]               = "500";
    static char cNotifBuf[128]           = "";
    static char cMsgBoxBuf[128]          = "";
    static char cAnimEventBuf[64]        = "JumpLand";
    static char cFuncName[128]           = "";
    static char cFuncParam1[128]         = "";
    static char cFuncParam2[128]         = "";
    static char cFuncParam3[128]         = "";
    static char cFuncParam4[128]         = "";
    static char cTitleSequenceName[128]  = "";
    static char cServerConsoleCmd[256]   = "";
    static char cLocalConsoleCmd[256]    = "";
    static char cDebugCellName[64]       = "";
    static bool bFreezeAI                = false;
    static bool bGhost                   = false;
    static bool bInChargen               = false;
    static bool bAlerted                 = false;
    static bool bUnconscious             = false;
    static bool bInvulnerable            = false;
    static bool bAvoidPlayer             = false;
    static bool bAttackOnSight           = false;
    static bool bPlayerControlsEnabled   = true;
    static bool bRestrained              = false;
    static bool bCanDoCommand            = false;
    static bool bCommandState            = false;
    static bool bGhostedToTeammates     = false;
    static bool bCharGenSkeleton         = false;
    static float fForceAngleX            = 0.0f;
    static float fForceAngleY            = 0.0f;
    static float fForceAngleZ            = 0.0f;
    static float fForceSpeed             = 1.0f;
    static float fForceSpeedRamp         = 1.0f;
    static float fForceRampTime          = 0.5f;
    static float fForceDirX              = 0.0f;
    static float fForceDirY              = 0.0f;
    static float fForceDirZ              = 1.0f;
    static char  cSubGraphAnim[64]       = "";
    static float fScale                  = 1.0f;
    static float fAlpha                  = 1.0f;
    static float fPushForce              = 40.0f;
    static char  cPlaceFormId[16]        = "";
    static bool  bDontMove               = false;
    static bool  bPlayerAIDriven         = false;
    static bool  bDebugCollisions        = true;
    static bool  bDebugMenus             = true;
    static bool  bPipboyHDRMask          = false;
    static bool  bDisableSSR             = false;
    static bool  bDisableDirLight        = false;
    static bool  bShowFirstPersonGeom    = true;
    static int   iPassTimeHours          = 1;
    static float fCtrlSmallMotor         = 0.5f;
    static float fCtrlBigMotor           = 0.5f;
    static float fCtrlDuration           = 0.5f;
    static int   iScreenBloodIntensity   = 50;
    static int   iCharGenHudMode         = 0;

    float scriptWidth = ImGui::GetContentRegionAvail().x * 0.5f;

    ImGui::Text("request player spell");
    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputText("##AdminSpellFormId", cSpellFormid, IM_ARRAYSIZE(cSpellFormid),
                     ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    if (ImGui::Button("request spell")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            std::uint32_t spellFormId = 0;
            if (::sscanf_s(cSpellFormid, "%x", &spellFormId) == 1) {
                g_ScriptSpellFormId = spellFormId;
                (void)CallFunction::applyCombatSpell(base, reinterpret_cast<void*>(lpActor), spellFormId);
            }
        }
    }

    ImGui::Text("send console command to equip any item");
    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputText("##AdminItemFormId", cItemFormid, IM_ARRAYSIZE(cItemFormid),
                     ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    if (ImGui::Button("request item")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            std::uint32_t itemFormId = 0;
            if (::sscanf_s(cItemFormid, "%x", &itemFormId) == 1) {
                (void)CallFunction::equipItem(base, reinterpret_cast<void*>(lpActor), itemFormId);
            }
        }
    }

    ImGui::Text("center on any cell (editor ID)");
    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputText("##AdminCellEditorId", cCellFormid, IM_ARRAYSIZE(cCellFormid));
    ImGui::SameLine();
    if (ImGui::Button("center on cell")) {
        uintptr_t base = getbase();
        (void)CallFunction::centerOnCell(base, cCellFormid);
    }

    ImGui::Separator();
    ImGui::Text("execute server dteleport");

    static float fPosX = 0.0f;
    static float fPosY = 0.0f;
    static float fPosZ = 0.0f;
    static float fYaw  = 0.0f;

    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputFloat("X", &fPosX, 0.0f, 0.0f, "%.3f");
    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputFloat("Y", &fPosY, 0.0f, 0.0f, "%.3f");
    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputFloat("Z", &fPosZ, 0.0f, 0.0f, "%.3f");
    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputFloat("Yaw", &fYaw, 0.0f, 0.0f, "%.3f");
    if (ImGui::Button("apply position/angle")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            (void)CallFunction::setPosition(base, reinterpret_cast<void*>(lpActor), fPosX, fPosY, fPosZ, fYaw);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("read current position/angle")) {
        ESP::Vec3 pos = ESP::getLocPlayerXYZ();
        float pitch = 0.0f, yawView = 0.0f;
        if (ESP::getViewAngles(pitch, yawView)) {
            fYaw = yawView;
        }
        fPosX = pos.x;
        fPosY = pos.y;
        fPosZ = pos.z;
    }


    ImGui::Separator();

    if (ImGui::Button("kill actor")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            (void)CallFunction::killActor(base, reinterpret_cast<void*>(lpActor));
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("silent kill")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            const char* params[] = { "None" };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.KillSilent",
                reinterpret_cast<void*>(lpActor),
                params,
                1
            );
        }
    }
    if (ImGui::Button("resurrect actor")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            const char* params[] = { "false", "false" };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.Resurrect",
                reinterpret_cast<void*>(lpActor),
                params,
                2
            );
        }
    }
    if (toggleswap("Alert", bAlerted)) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            const char* params[] = { bAlerted ? "true" : "false" };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.SetAlert",
                reinterpret_cast<void*>(lpActor),
                params,
                1
            );
        }
    }

    if (toggleswap("Ghost", bGhost)) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            (void)CallFunction::setGhost(base, reinterpret_cast<void*>(lpActor), bGhost);
        }
    }

    if (toggleswap("Freeze AI", bFreezeAI)) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            if (bFreezeAI) {
                (void)CallFunction::enableAI(base, reinterpret_cast<void*>(lpActor), false, true);
            } else {
                (void)CallFunction::enableAI(base, reinterpret_cast<void*>(lpActor), true, false);
            }
        }
    }

    if (toggleswap("Unconscious", bUnconscious)) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            const char* params[] = { bUnconscious ? "true" : "false" };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.SetUnconscious",
                reinterpret_cast<void*>(lpActor),
                params,
                1
            );
        }
    }

    if (toggleswap("Invulnerable", bInvulnerable)) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            const char* params[] = { bInvulnerable ? "true" : "false" };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.SetInvulnerable",
                reinterpret_cast<void*>(lpActor),
                params,
                1
            );
        }
    }

    if (toggleswap("Avoid Player", bAvoidPlayer)) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            const char* params[] = { bAvoidPlayer ? "true" : "false" };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.SetAvoidPlayer",
                reinterpret_cast<void*>(lpActor),
                params,
                1
            );
        }
    }

    if (toggleswap("Attack On Sight", bAttackOnSight)) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            const char* params[] = { bAttackOnSight ? "true" : "false" };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.SetAttackActorOnSight",
                reinterpret_cast<void*>(lpActor),
                params,
                1
            );
        }
    }

    if (toggleswap("Ghost to teammates", bGhostedToTeammates)) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            const char* params[] = { bGhostedToTeammates ? "true" : "false" };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.SetGhostedToTeammates",
                reinterpret_cast<void*>(lpActor),
                params,
                1
            );
        }
    }

    if (toggleswap("Player controls enabled", bPlayerControlsEnabled)) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            const char* params[] = { bPlayerControlsEnabled ? "true" : "false" };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.SetPlayerControls",
                reinterpret_cast<void*>(lpActor),
                params,
                1
            );
        }
    }

    if (toggleswap("Restrained", bRestrained)) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            const char* params[] = { bRestrained ? "true" : "false" };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.SetRestrained",
                reinterpret_cast<void*>(lpActor),
                params,
                1
            );
        }
    }

    if (toggleswap("Can Do Command", bCanDoCommand)) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            const char* params[] = { bCanDoCommand ? "true" : "false" };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.SetCanDoCommand",
                reinterpret_cast<void*>(lpActor),
                params,
                1
            );
        }
    }

    if (toggleswap("Command State", bCommandState)) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            const char* params[] = { bCommandState ? "true" : "false" };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.SetCommandState",
                reinterpret_cast<void*>(lpActor),
                params,
                1
            );
        }
    }

    if (ImGui::Button("Stop combat")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.StopCombat",
                reinterpret_cast<void*>(lpActor),
                nullptr,
                0
            );
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop combat alarm")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.StopCombatAlarm",
                reinterpret_cast<void*>(lpActor),
                nullptr,
                0
            );
        }
    }

    if (ImGui::Button("Start sneaking")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.StartSneaking",
                reinterpret_cast<void*>(lpActor),
                nullptr,
                0
            );
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Exit Power Armor")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.ExitPowerArmor",
                reinterpret_cast<void*>(lpActor),
                nullptr,
                0
            );
        }
    }

    if (toggleswap("CharGen skeleton", bCharGenSkeleton)) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            const char* params[] = { bCharGenSkeleton ? "true" : "false" };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.SetHasCharGenSkeleton",
                reinterpret_cast<void*>(lpActor),
                params,
                1
            );
        }
    }

    ImGui::Separator();

    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputFloat("##AdminScale", &fScale, 0.0f, 0.0f, "ss %.2f");
    ImGui::SameLine();
    if (ImGui::Button("set scale")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            char scaleBuf[32];
            std::snprintf(scaleBuf, sizeof(scaleBuf), "%.6f", static_cast<double>(fScale));
            const char* params[] = { scaleBuf };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.SetScale",
                reinterpret_cast<void*>(lpActor),
                params,
                1
            );
        }
    }

    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputFloat("##AdminAlpha", &fAlpha, 0.0f, 0.0f, "a %.2f");
    ImGui::SameLine();
    if (ImGui::Button("set alpha")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            (void)CallFunction::setAlpha(base, reinterpret_cast<void*>(lpActor), fAlpha);
        }
    }


    ImGui::Text("spells");

    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputText("##AdminPerkFormId", cPerkFormid, IM_ARRAYSIZE(cPerkFormid),
                     ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    if (ImGui::Button("add perk")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            std::uint32_t perkFormId = 0;
            if (::sscanf_s(cPerkFormid, "%x", &perkFormId) == 1) {
                (void)CallFunction::addPerk(base, reinterpret_cast<void*>(lpActor), perkFormId, false);
            }
        }
    }

    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputText("##AdminAddSpellFormId", cAddSpellFormid, IM_ARRAYSIZE(cAddSpellFormid),
                     ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    if (ImGui::Button("add spell")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            std::uint32_t spellFormId = 0;
            if (::sscanf_s(cAddSpellFormid, "%x", &spellFormId) == 1) {
                (void)CallFunction::addSpell(base, reinterpret_cast<void*>(lpActor), spellFormId, true);
            }
        }
    }

    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputText("##AdminIdleFormId", cIdleFormid, IM_ARRAYSIZE(cIdleFormid),
                     ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    if (ImGui::Button("play idle by fid")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal) {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            std::uint32_t idleFormId = 0;
            if (::sscanf_s(cIdleFormid, "%x", &idleFormId) == 1) {
                (void)CallFunction::playIdle(base, reinterpret_cast<void*>(lpActor), idleFormId);
            }
        }
    }

    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputText("##AdminIdleName", cIdleName, IM_ARRAYSIZE(cIdleName));
    ImGui::SameLine();
    if (ImGui::Button("play idle by name")) {
        uintptr_t base = getbase();
        uintptr_t lpGlobal = getlocal_player();
        if (base && lpGlobal && cIdleName[0] != '\0') {
            uintptr_t lpActor = *reinterpret_cast<uintptr_t*>(lpGlobal);
            const char* params[] = { cIdleName };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Actor.PlayIdle",
                reinterpret_cast<void*>(lpActor),
                params,
                1
            );
        }
    }

    ImGui::Separator();

    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputText("##ServerConsoleCmd", cServerConsoleCmd, IM_ARRAYSIZE(cServerConsoleCmd));
    ImGui::SameLine();
    if (ImGui::Button("Exec server console")) {
        uintptr_t base = getbase();
        if (base && cServerConsoleCmd[0] != '\0') {
            const char* params[] = { cServerConsoleCmd };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Debug.ExecuteServerConsole",
                nullptr,
                params,
                1
            );
        }
    }

    ImGui::SetNextItemWidth(scriptWidth);
    ImGui::InputText("##LocalConsoleCmd", cLocalConsoleCmd, IM_ARRAYSIZE(cLocalConsoleCmd));
    ImGui::SameLine();
    if (ImGui::Button("Exec local console")) {
        uintptr_t base = getbase();
        if (base && cLocalConsoleCmd[0] != '\0') {
            const char* params[] = { cLocalConsoleCmd };
            (void)CallFunction::callPapyrusFunction(
                base,
                "Debug.ExecuteLocalConsole",
                nullptr,
                params,
                1
            );
        }
    }
}

void renderAdminTab() {
    ImGui::Spacing();
    AdminSectionHeader("TEST1");
    ImGui::TextWrapped(
        "2726892762872gjhv2hjg4");
}