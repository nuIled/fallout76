#include "../menu_main/gui.h"
#include "../../features/esp/esp_main/esp.h"
#include "../../../libs/imgui/imgui.h"

static bool toggleswap(const char* label, bool& state) {
    bool changed = false;
    ImGui::PushID(label);

    float switchWidth = 50.0f;
    float switchHeight = 24.0f;
    float knobSize = switchHeight - 4.0f;
    float padding = 2.0f;

    ImGui::Text("%s", label);
    float labelWidth = ImGui::GetItemRectSize().x;
    ImGui::SameLine(labelWidth + 20.0f);

    ImVec2 switchPos = ImGui::GetCursorScreenPos();
    ImVec2 switchSize(switchWidth, switchHeight);

    ImGui::InvisibleButton("##switch", switchSize);
    if (ImGui::IsItemClicked()) {
        state = !state;
        changed = true;
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImU32 bgColor = state ? IM_COL32(255, 255, 255, 255) : IM_COL32(80, 80, 80, 255);
    ImU32 knobColor = state ? IM_COL32(0, 0, 0, 255) : IM_COL32(255, 255, 255, 255);
    
    drawList->AddRectFilled(
        switchPos, 
        ImVec2(switchPos.x + switchWidth, switchPos.y + switchHeight), 
        bgColor, 
        switchHeight * 0.5f
    );
    
    float knobX = state ? (switchPos.x + switchWidth - knobSize - padding) : (switchPos.x + padding);
    float knobY = switchPos.y + switchHeight * 0.5f;
    drawList->AddCircleFilled(
        ImVec2(knobX + knobSize * 0.5f, knobY), 
        knobSize * 0.5f, 
        knobColor
    );

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + switchHeight);
    
    ImGui::PopID();
    
    return changed;
}

static void EspSectionHeader(const char* label) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    ImVec2 p = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(p.x + 1, p.y + 1));
    ImGui::Text("%s", label);
    ImGui::SetCursorPos(p);
    ImGui::Text("%s", label);
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();
}

void renderESPTab() {
    EspSectionHeader("ESP");
    toggleswap("Enable ESP", ESP::Config::enable);
    static bool showESPDebug = false;
    toggleswap("Enable ESP debug", showESPDebug);
    if (!ESP::Config::enable) {
        ImGui::BeginDisabled();
    }
    ImGui::Spacing();
    EspSectionHeader("ESP Settings:");
    toggleswap("Enable snaplines", ESP::Config::showSnaplines);
    toggleswap("Enable Entity info", ESP::Config::showEntityInfo);
    ImGui::Spacing();
    EspSectionHeader("ESP Customisation");
    toggleswap("Show Players", ESP::Config::showPlayers);
    toggleswap("Show NPCs", ESP::Config::showNPCs);
    toggleswap("Show Containers", ESP::Config::showContainers);
    toggleswap("Show Junk", ESP::Config::showJunk);
    toggleswap("Show Plans", ESP::Config::showPlans);
    toggleswap("Show Magazines", ESP::Config::showMagazines);
    toggleswap("Show Bobbleheads", ESP::Config::showBobbleheads);
    toggleswap("Show Flora", ESP::Config::showFlora);
    toggleswap("Show Weapons", ESP::Config::showWeapons);
    toggleswap("Show Armor", ESP::Config::showArmor);
    toggleswap("Show Ammo", ESP::Config::showAmmo);
    toggleswap("Show Aid", ESP::Config::showAid);
    toggleswap("Show Misc", ESP::Config::showMisc);
    toggleswap("Show Mods", ESP::Config::showMods);
    toggleswap("Show Other", ESP::Config::showOther);
    if (showESPDebug) {
        ImGui::Spacing();
        EspSectionHeader("ESP Box extra");
        float sliderWidth = ImGui::GetContentRegionAvail().x * 0.5f;
        ImGui::Text("Max Distance:");
        ImGui::SetNextItemWidth(sliderWidth);
        ImGui::SliderFloat("##maxDistance", &ESP::Config::maxDistance, 10.0f, 1000.0f, "%.0f units");
        ImGui::Text("FOV:");
        ImGui::SetNextItemWidth(sliderWidth);
        ImGui::SliderFloat("##fov", &ESP::Config::fov, 30.0f, 150.0f, "%.1f°");
        ImGui::Text("Box Horizontal Scale:");
        ImGui::SetNextItemWidth(sliderWidth);
        ImGui::SliderFloat("##boxScaleX", &ESP::Config::boxScaleX, 0.1f, 3.0f, "%.2f");
        ImGui::Text("Box Vertical Scale:");
        ImGui::SetNextItemWidth(sliderWidth);
        ImGui::SliderFloat("##boxScaleY", &ESP::Config::boxScaleY, 0.1f, 3.0f, "%.2f");
        ImGui::Text("Box Horizontal Offset:");
        ImGui::SetNextItemWidth(sliderWidth);
        ImGui::SliderFloat("##boxOffsetX", &ESP::Config::boxOffsetX, -200.0f, 200.0f, "%.0f px");
        ImGui::Text("Box Vertical Offset:");
        ImGui::SetNextItemWidth(sliderWidth);
        ImGui::SliderFloat("##boxOffsetY", &ESP::Config::boxOffsetY, -200.0f, 200.0f, "%.0f px");
        ImGui::Text("Container vertical offset:");
        ImGui::SetNextItemWidth(sliderWidth);
        ImGui::SliderFloat("##containerScreenOffsetY", &ESP::Config::containerScreenOffsetY, -80.0f, 80.0f, "%.0f px");
        toggleswap("Dynamic Box Scaling", ESP::Config::dynamicBoxScaling);
        if (ESP::Config::dynamicBoxScaling) {
            ImGui::Text("Min Scale (Far):");
            ImGui::SetNextItemWidth(sliderWidth);
            ImGui::SliderFloat("##minBoxScale", &ESP::Config::minBoxScale, 0.1f, 1.0f, "%.2f");
            ImGui::Text("Max Scale (Close):");
            ImGui::SetNextItemWidth(sliderWidth);
            ImGui::SliderFloat("##maxBoxScale", &ESP::Config::maxBoxScale, 1.0f, 3.0f, "%.2f");
            ImGui::Text("Scaling Distance:");
            ImGui::SetNextItemWidth(sliderWidth);
            ImGui::SliderFloat("##scalingDistance", &ESP::Config::scalingDistance, 100.0f, 5000.0f, "%.0f units");
        }
    }
    ImGui::Spacing();
    EspSectionHeader("ESP Colour Customisation");
    ImGui::Text("Player Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 playerColorVec = ImVec4(
        ESP::Config::playerColor.Value.x,
        ESP::Config::playerColor.Value.y,
        ESP::Config::playerColor.Value.z,
        ESP::Config::playerColor.Value.w
    );
    if (ImGui::ColorEdit4("##playerColour", (float*)&playerColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::playerColor = ImColor(playerColorVec);
    }

    ImGui::Text("NPC Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 npcColorVec = ImVec4(
        ESP::Config::npcColor.Value.x,
        ESP::Config::npcColor.Value.y,
        ESP::Config::npcColor.Value.z,
        ESP::Config::npcColor.Value.w
    );
    if (ImGui::ColorEdit4("##npcColour", (float*)&npcColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::npcColor = ImColor(npcColorVec);
    }

    ImGui::Text("Enemy Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 enemyColorVec = ImVec4(
        ESP::Config::enemyColor.Value.x,
        ESP::Config::enemyColor.Value.y,
        ESP::Config::enemyColor.Value.z,
        ESP::Config::enemyColor.Value.w
    );
    if (ImGui::ColorEdit4("##enemyColour", (float*)&enemyColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::enemyColor = ImColor(enemyColorVec);
    }

    ImGui::Text("Container Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 containerColorVec = ImVec4(
        ESP::Config::containerColor.Value.x,
        ESP::Config::containerColor.Value.y,
        ESP::Config::containerColor.Value.z,
        ESP::Config::containerColor.Value.w
    );
    if (ImGui::ColorEdit4("##containerColour", (float*)&containerColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::containerColor = ImColor(containerColorVec);
    }

    ImGui::Text("Junk Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 junkColorVec = ImVec4(
        ESP::Config::junkColor.Value.x,
        ESP::Config::junkColor.Value.y,
        ESP::Config::junkColor.Value.z,
        ESP::Config::junkColor.Value.w
    );
    if (ImGui::ColorEdit4("##junkColour", (float*)&junkColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::junkColor = ImColor(junkColorVec);
    }

    ImGui::Text("Plan Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 planColorVec = ImVec4(
        ESP::Config::planColor.Value.x,
        ESP::Config::planColor.Value.y,
        ESP::Config::planColor.Value.z,
        ESP::Config::planColor.Value.w
    );
    if (ImGui::ColorEdit4("##planColour", (float*)&planColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::planColor = ImColor(planColorVec);
    }

    ImGui::Text("Magazine Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 magazineColorVec = ImVec4(
        ESP::Config::magazineColor.Value.x,
        ESP::Config::magazineColor.Value.y,
        ESP::Config::magazineColor.Value.z,
        ESP::Config::magazineColor.Value.w
    );
    if (ImGui::ColorEdit4("##magazineColour", (float*)&magazineColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::magazineColor = ImColor(magazineColorVec);
    }

    ImGui::Text("Bobblehead Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 bobbleheadColorVec = ImVec4(
        ESP::Config::bobbleheadColor.Value.x,
        ESP::Config::bobbleheadColor.Value.y,
        ESP::Config::bobbleheadColor.Value.z,
        ESP::Config::bobbleheadColor.Value.w
    );
    if (ImGui::ColorEdit4("##bobbleheadColour", (float*)&bobbleheadColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::bobbleheadColor = ImColor(bobbleheadColorVec);
    }

    ImGui::Text("Flora Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 floraColorVec = ImVec4(
        ESP::Config::floraColor.Value.x,
        ESP::Config::floraColor.Value.y,
        ESP::Config::floraColor.Value.z,
        ESP::Config::floraColor.Value.w
    );
    if (ImGui::ColorEdit4("##floraColour", (float*)&floraColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::floraColor = ImColor(floraColorVec);
    }

    ImGui::Text("Weapon Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 weaponColorVec = ImVec4(
        ESP::Config::weaponColor.Value.x,
        ESP::Config::weaponColor.Value.y,
        ESP::Config::weaponColor.Value.z,
        ESP::Config::weaponColor.Value.w
    );
    if (ImGui::ColorEdit4("##weaponColour", (float*)&weaponColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::weaponColor = ImColor(weaponColorVec);
    }

    ImGui::Text("Armor Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 armorColorVec = ImVec4(
        ESP::Config::armorColor.Value.x,
        ESP::Config::armorColor.Value.y,
        ESP::Config::armorColor.Value.z,
        ESP::Config::armorColor.Value.w
    );
    if (ImGui::ColorEdit4("##armorColour", (float*)&armorColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::armorColor = ImColor(armorColorVec);
    }

    ImGui::Text("Ammo Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 ammoColorVec = ImVec4(
        ESP::Config::ammoColor.Value.x,
        ESP::Config::ammoColor.Value.y,
        ESP::Config::ammoColor.Value.z,
        ESP::Config::ammoColor.Value.w
    );
    if (ImGui::ColorEdit4("##ammoColour", (float*)&ammoColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::ammoColor = ImColor(ammoColorVec);
    }

    ImGui::Text("Aid Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 aidColorVec = ImVec4(
        ESP::Config::aidColor.Value.x,
        ESP::Config::aidColor.Value.y,
        ESP::Config::aidColor.Value.z,
        ESP::Config::aidColor.Value.w
    );
    if (ImGui::ColorEdit4("##aidColour", (float*)&aidColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::aidColor = ImColor(aidColorVec);
    }

    ImGui::Text("Misc Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 miscColorVec = ImVec4(
        ESP::Config::miscColor.Value.x,
        ESP::Config::miscColor.Value.y,
        ESP::Config::miscColor.Value.z,
        ESP::Config::miscColor.Value.w
    );
    if (ImGui::ColorEdit4("##miscColour", (float*)&miscColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::miscColor = ImColor(miscColorVec);
    }

    ImGui::Text("Mod Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 modColorVec = ImVec4(
        ESP::Config::modColor.Value.x,
        ESP::Config::modColor.Value.y,
        ESP::Config::modColor.Value.z,
        ESP::Config::modColor.Value.w
    );
    if (ImGui::ColorEdit4("##modColour", (float*)&modColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::modColor = ImColor(modColorVec);
    }

    ImGui::Text("Other Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 otherColorVec = ImVec4(
        ESP::Config::otherColor.Value.x,
        ESP::Config::otherColor.Value.y,
        ESP::Config::otherColor.Value.z,
        ESP::Config::otherColor.Value.w
    );
    if (ImGui::ColorEdit4("##otherColour", (float*)&otherColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::otherColor = ImColor(otherColorVec);
    }

    ImGui::Text("Name Colour:");
    ImGui::SetNextItemWidth(-1);
    ImVec4 nameColorVec = ImVec4(
        ESP::Config::nameColor.Value.x,
        ESP::Config::nameColor.Value.y,
        ESP::Config::nameColor.Value.z,
        ESP::Config::nameColor.Value.w
    );
    if (ImGui::ColorEdit4("##nameColour", (float*)&nameColorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        ESP::Config::nameColor = ImColor(nameColorVec);
    }
    
    if (!ESP::Config::enable) {
        ImGui::EndDisabled();
    }
}