#include "../esp_main/esp_internal.h"
#include "imgui/imgui.h"
#include <cmath>
#include <cstdio>
#include <cfloat>

namespace ESP {
    void RenderESP() {
        if (!Config::enable) {
            return;
        }

        float screenW = 1920.0f;
        float screenH = 1080.0f;
        getScreenSize(screenW, screenH);
        float halfScreenW = screenW * 0.5f;
        float halfScreenH = screenH * 0.5f;
        auto drawList = ImGui::GetBackgroundDrawList();
        const float screenMargin = 50.0f;
        const float minScreenX = -screenMargin;
        const float maxScreenX = screenW + screenMargin;
        const float minScreenY = -screenMargin;
        const float maxScreenY = screenH + screenMargin;

        auto renderEntity = [&](const EntityData& ent, const ImColor& boxColor) {
            if (ent.distanceM > static_cast<int>(Config::maxDistance)) {
                return;
            }
            if (ent.position.x == 0.0f && ent.position.y == 0.0f && ent.position.z == 0.0f) {
                return;
            }
            Vec2 anchorScreenPos = ent.screenPos;
            bool onScreen = ent.onScreen;
            if ((anchorScreenPos.x == 0.0f && anchorScreenPos.y == 0.0f) || !onScreen) {
                onScreen = false;
                Vec2 recalcPos = W2S(ent.position, screenW, screenH, onScreen);
                if (recalcPos.x != 0.0f || recalcPos.y != 0.0f || onScreen) {
                    anchorScreenPos = recalcPos;
                } else {
                    return;
                }
            }
            if (anchorScreenPos.x < -1000.0f || anchorScreenPos.x > screenW + 1000.0f ||
                anchorScreenPos.y < -1000.0f || anchorScreenPos.y > screenH + 1000.0f) {
                return;
            }
            float anchorX = anchorScreenPos.x + Config::boxOffsetX;
            float anchorY = anchorScreenPos.y + Config::boxOffsetY;
            if (ent.category == EntityCategory::Container)
                anchorY += Config::containerScreenOffsetY;
            float distanceScale = 1.0f;
            float distance = static_cast<float>(ent.distanceM);

            if (Config::dynamicBoxScaling) {
                float minScale = Config::minBoxScale;
                float maxDistance = (Config::scalingDistance > 10.0f) ? Config::scalingDistance : 50.0f;
                // you can customise this if you like
                if (distance <= 10.0f) {
                    distanceScale = 1.0f;
                } else if (distance >= maxDistance) {
                    distanceScale = minScale;
                } else {
                    float t = (distance - 10.0f) / (maxDistance - 10.0f);
                    distanceScale = 1.0f - t * (1.0f - minScale);
                }
            } else {
                const float minScale = 0.1f;
                const float maxDistance = 40.0f;

                if (distance <= 10.0f) {
                    distanceScale = 1.0f;
                } else if (distance >= maxDistance) {
                    distanceScale = minScale;
                } else {
                    float t = (distance - 10.0f) / (maxDistance - 10.0f);
                    distanceScale = 1.0f - t * (1.0f - minScale);
                }
            }
            float totalScaleX = Config::boxScaleX * distanceScale;
            float totalScaleY = Config::boxScaleY * distanceScale;
            const float baseSize = 12.0f;
            const float boxWidth = baseSize * totalScaleX;
            const float boxHeight = baseSize * totalScaleY;
            const float boxX = anchorX - boxWidth * 0.5f;
            const float boxY = anchorY - boxHeight * 0.5f;
            const float adjustedBoxWidth = boxWidth;
            const float adjustedBoxHeight = boxHeight;

            if (Config::showSnaplines) {
                const ImVec2 from(halfScreenW, halfScreenH);
                const ImVec2 to(anchorX, anchorY);
                drawList->AddLine(from, to, boxColor, 1.5f);
            }
            if (Config::showBoxes) {
                const float thickness = 2.0f;
                const float cornerLength = adjustedBoxWidth * 0.3f;
            drawList->AddLine(
                ImVec2(boxX, boxY),
                ImVec2(boxX + cornerLength, boxY),
                boxColor,
                thickness
            );
            drawList->AddLine(
                ImVec2(boxX, boxY),
                ImVec2(boxX, boxY + cornerLength),
                boxColor,
                thickness
            );
            drawList->AddLine(
                ImVec2(boxX + adjustedBoxWidth, boxY),
                ImVec2(boxX + adjustedBoxWidth - cornerLength, boxY),
                boxColor,
                thickness
            );
            drawList->AddLine(
                ImVec2(boxX + adjustedBoxWidth, boxY),
                ImVec2(boxX + adjustedBoxWidth, boxY + cornerLength),
                boxColor,
                thickness
            );
            drawList->AddLine(
                ImVec2(boxX, boxY + adjustedBoxHeight),
                ImVec2(boxX + cornerLength, boxY + adjustedBoxHeight),
                boxColor,
                thickness
            );
            drawList->AddLine(
                ImVec2(boxX, boxY + adjustedBoxHeight),
                ImVec2(boxX, boxY + adjustedBoxHeight - cornerLength),
                boxColor,
                thickness
            );
            drawList->AddLine(
                ImVec2(boxX + adjustedBoxWidth, boxY + adjustedBoxHeight),
                ImVec2(boxX + adjustedBoxWidth - cornerLength, boxY + adjustedBoxHeight),
                boxColor,
                thickness
            );
            drawList->AddLine(
                ImVec2(boxX + adjustedBoxWidth, boxY + adjustedBoxHeight),
                ImVec2(boxX + adjustedBoxWidth, boxY + adjustedBoxHeight - cornerLength),
                boxColor,
                thickness
            );
            }

            if (Config::showEntityInfo) {
                ImFont* espFont = nullptr;
                ImGuiIO& io = ImGui::GetIO();
                if (io.Fonts->Fonts.Size > 1)
                    espFont = io.Fonts->Fonts[1];
                if (!espFont)
                    espFont = ImGui::GetFont();
                float baseFontScale = Config::showBoxes ? 1.0f : 1.5f;// edit scaling here
                const float d = static_cast<float>(ent.distanceM < 0 ? 0 : ent.distanceM);
                float infoDistanceScale;
                if (d <= 5.0f) {
                    infoDistanceScale = 1.2f - (d / 5.0f) * (1.2f - 0.975f);
                } else if (d <= 15.0f) {
                    infoDistanceScale = 0.975f - ((d - 5.0f) / 10.0f) * (0.975f - 0.6f);
                } else if (d <= 30.0f) {
                    const float u = (d - 15.0f) / 15.0f;
                    infoDistanceScale = 0.6f + u * (0.72f - 0.6f);
                } else {
                    float t = (d - 30.0f) / 90.0f;
                    if (t > 1.0f) t = 1.0f;
                    infoDistanceScale = 0.72f - t * (0.72f - 0.24f);
                }
                float fontScale = baseFontScale * infoDistanceScale;
                std::string displayName = ent.name;
                if (displayName.empty() && ent.isPlayer)
                    displayName = "Player";
                else if (displayName.empty() && ent.isNPC)
                    displayName = "NPC";
                std::string healthText;
                if ((ent.isPlayer || ent.isNPC) && ent.maxHealth > 0.0f) {
                    float healthPercent = (ent.health / ent.maxHealth) * 100.0f;
                    if (healthPercent < 0.0f) healthPercent = 0.0f;
                    if (healthPercent > 100.0f) healthPercent = 100.0f;
                    char healthPart[48];
                    if (ent.health <= 0.0f) {
                        snprintf(healthPart, sizeof(healthPart), "dead [%dm]", ent.distanceM);
                    } else {
                        snprintf(healthPart, sizeof(healthPart), "%.0fhp [%dm]", healthPercent, ent.distanceM);
                    }
                    healthText = healthPart;
                }

                std::string belowBoxText = healthText;
                if (belowBoxText.empty() && !displayName.empty()) {
                    char distOnly[32];
                    snprintf(distOnly, sizeof(distOnly), "%dm", ent.distanceM);
                    belowBoxText = distOnly;
                }
                constexpr float kEspNominalPx = 14.0f;
                const float scaledFontSize = kEspNominalPx * fontScale;
                float currentY = boxY + adjustedBoxHeight + 4.0f;

                if (!displayName.empty()) {
                    ImVec2 textSize = espFont
                        ? espFont->CalcTextSizeA(scaledFontSize, FLT_MAX, 0.0f, displayName.c_str())
                        : ImGui::CalcTextSize(displayName.c_str());
                    drawList->AddText(
                        espFont,
                        scaledFontSize,
                        ImVec2(anchorX - textSize.x * 0.5f, boxY - textSize.y - 4.0f),
                        boxColor,
                        displayName.c_str()
                    );
                }

                if (!belowBoxText.empty()) {
                    ImVec2 infoTextSize = espFont
                        ? espFont->CalcTextSizeA(scaledFontSize, FLT_MAX, 0.0f, belowBoxText.c_str())
                        : ImGui::CalcTextSize(belowBoxText.c_str());
                    drawList->AddText(
                        espFont,
                        scaledFontSize,
                        ImVec2(anchorX - infoTextSize.x * 0.5f, currentY),
                        boxColor,
                        belowBoxText.c_str()
                    );
                }
            }

            if (Config::showHealthBar && (ent.isPlayer || ent.isNPC) && ent.maxHealth > 0.0f) {/* this is remove since its non compat with recent changes. total rewrite would be needed.*/
                float healthPercent = (ent.health / ent.maxHealth) * 100.0f;
                if (healthPercent < 0.0f) healthPercent = 0.0f;
                if (healthPercent > 100.0f) healthPercent = 100.0f;
                const float healthBarWidth = 4.0f;
                const float healthBarSpacing = 3.0f;
                float healthBarX = boxX + adjustedBoxWidth + healthBarSpacing;
                float healthBarY = boxY;
                float healthBarHeight = adjustedBoxHeight;
                float filledHeight = (healthBarHeight * healthPercent) / 100.0f;
                float emptyHeight = healthBarHeight - filledHeight;

                ImColor healthBarColor;
                if (healthPercent > 50.0f) {
                    float t = (healthPercent - 50.0f) / 50.0f;
                    healthBarColor = ImColor(
                        static_cast<int>(255 * (1.0f - t)),
                        255,
                        0,
                        255
                    );
                } else if (healthPercent > 25.0f) {
                    float t = (healthPercent - 25.0f) / 25.0f;
                    healthBarColor = ImColor(
                        255,
                        static_cast<int>(255 - 127 * (1.0f - t)),
                        0,
                        255
                    );
                } else {
                    float t = healthPercent / 25.0f;
                    healthBarColor = ImColor(
                        255,
                        static_cast<int>(128 * t),
                        0,
                        255
                    );
                }
                drawList->AddRectFilled(
                    ImVec2(healthBarX, healthBarY),
                    ImVec2(healthBarX + healthBarWidth, healthBarY + healthBarHeight),
                    ImColor(50, 50, 50, 255),
                    0.0f
                );
                if (filledHeight > 0.0f) {
                    drawList->AddRectFilled(
                        ImVec2(healthBarX, healthBarY + emptyHeight),
                        ImVec2(healthBarX + healthBarWidth, healthBarY + healthBarHeight),
                        healthBarColor,
                        0.0f
                    );
                }
                drawList->AddRect(
                    ImVec2(healthBarX, healthBarY),
                    ImVec2(healthBarX + healthBarWidth, healthBarY + healthBarHeight),
                    ImColor(0, 0, 0, 255),
                    0.0f,
                    0,
                    1.0f
                );
            }
        };

        if (Config::showPlayers) {
            for (const auto& player : g_players) {
                renderEntity(player, Config::playerColor);
            }
        }
        if (Config::showNPCs) {
            for (const auto& npc : g_npcs) {
                const ImColor npcTint = npc.isEnemy ? Config::enemyColor : Config::npcColor;
                renderEntity(npc, npcTint);
            }
        }

        for (const auto& ent : g_entities) {
            bool shouldRender = false;
            ImColor color = Config::otherColor;
            switch (static_cast<int>(ent.category)) {
                case static_cast<int>(EntityCategory::Container):
                    shouldRender = Config::showContainers;
                    color = Config::containerColor;
                    break;
                case static_cast<int>(EntityCategory::Junk):
                    shouldRender = Config::showJunk;
                    color = Config::junkColor;
                    break;
                case static_cast<int>(EntityCategory::Plan):
                    shouldRender = Config::showPlans;
                    color = Config::planColor;
                    break;
                case static_cast<int>(EntityCategory::Magazine):
                    shouldRender = Config::showMagazines;
                    color = Config::magazineColor;
                    break;
                case static_cast<int>(EntityCategory::Bobblehead):
                    shouldRender = Config::showBobbleheads;
                    color = Config::bobbleheadColor;
                    break;
                case static_cast<int>(EntityCategory::Flora):
                    shouldRender = Config::showFlora;
                    color = Config::floraColor;
                    break;
                case static_cast<int>(EntityCategory::Weapon):
                    shouldRender = Config::showWeapons;
                    color = Config::weaponColor;
                    break;
                case static_cast<int>(EntityCategory::Armor):
                    shouldRender = Config::showArmor;
                    color = Config::armorColor;
                    break;
                case static_cast<int>(EntityCategory::Ammo):
                    shouldRender = Config::showAmmo;
                    color = Config::ammoColor;
                    break;
                case static_cast<int>(EntityCategory::Aid):
                    shouldRender = Config::showAid;
                    color = Config::aidColor;
                    break;
                case static_cast<int>(EntityCategory::Misc):
                    shouldRender = Config::showMisc;
                    color = Config::miscColor;
                    break;
                case static_cast<int>(EntityCategory::Mod):
                    shouldRender = Config::showMods;
                    color = Config::modColor;
                    break;
                case static_cast<int>(EntityCategory::Other):
                    shouldRender = Config::showOther;
                    color = Config::otherColor;
                    break;
                default:
                    break;
            }
            if (shouldRender) {
                renderEntity(ent, color);
            }
        }
    }
}