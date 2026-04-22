#include "../menu_main/gui.h"
#include "../../core/dll_main/globals.h"
#include "../../../libs/imgui/imgui.h"
#include "../../features/scripts_lader_date/call_function.h"
#include <cstdio>

float fcharsigma = 0.0f;
float fcharfat = 0.0f;
float fcharthin = 0.0f;
char cformidinput[16] = "00000000";
char cCustomOmodInput[16] = "00113689";
uint32_t uioriginalformid = 0;
uint32_t uicurrentformid = 0;
uint32_t uiCustomOmodVal = 0x00113689;
bool bisformidvalid = false;
bool fcharsizeloaded = false;
bool bCustomOmodValApply = true;
bool bcustomOmodEnabled = false;

bool toggleswap(const char* label, bool& state) {
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

static void SectionHeader(const char* label) {
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

void renderMainTab() {
    ImGui::Spacing();
    SectionHeader("MOVEMENT");

    if (toggleswap("speed hack", playerSpeedboostStart)) {}
    tooltips("Speed Hack", "sets your player speed to max possible");

    if (toggleswap("no clip", bnoclipenabled)) {
        if (!bnoclipenabled) {
            bNoclipEnabled = false;
            bnoclipon = false;
        }
    }
    tooltips("no clip", "Fallout 4 TCL. Mouse 4");

    if (bnoclipenabled) {
        static bool g_ShowExtraDetails = false;
        toggleswap("noclip options", g_ShowExtraDetails);
        if (g_ShowExtraDetails) {
            toggleswap("toggle tcl", bnocliptoggle);
            ImGui::SameLine();
            ImGui::TextDisabled(bnocliptoggle ? "(Press M4)" : "(Hold M4)");
            float sliderValue = ((fNoclipSpeedMult - 0.005f) / 0.095f) * 100.0f;
            float sliderWidth = ImGui::GetContentRegionAvail().x * 0.5f;
            ImGui::SetNextItemWidth(sliderWidth);
            if (ImGui::SliderFloat("##fNoclipSpeed", &sliderValue, 0.0f, 100.0f, "speed: %.0f%%")) {
                fNoclipSpeedMult = 0.005f + (sliderValue / 100.0f) * 0.095f;
            }
        }
    }

    static bool g_SuperJumpToggle = false;
    static float g_SuperJumpSlider = 70.0f;

    if (toggleswap("super jump", g_SuperJumpToggle)) {
        if (!g_SuperJumpToggle) setSuperJump(70.0f);
        else setSuperJump(g_SuperJumpSlider);
    }
    tooltips("super jump", "gta 5 super jump, now in fallout 67!!");

    if (g_SuperJumpToggle) {
        float sliderWidthSJ = ImGui::GetContentRegionAvail().x * 0.5f;
        ImGui::SetNextItemWidth(sliderWidthSJ);
        if (ImGui::SliderFloat("##SuperJumpSlider", &g_SuperJumpSlider, 70.0f, 10000.0f, "%.1f")) {
            setSuperJump(g_SuperJumpSlider);
        }
    }
    static bool g_JetpackHeightToggle = false;
    static float g_JetpackHeightSlider = 750.0f;

    if (toggleswap("jetpack modifier", g_JetpackHeightToggle)) {
        if (!g_JetpackHeightToggle) setJetpackHeight(750.0f);
        else setJetpackHeight(g_JetpackHeightSlider);
    }
    tooltips("jetpack boost!", "modify jetpack boost strength");
    if (g_JetpackHeightToggle) {
        float sliderWidthJet = ImGui::GetContentRegionAvail().x * 0.5f;
        ImGui::SetNextItemWidth(sliderWidthJet);
        if (ImGui::SliderFloat("##JetpackHeightSlider", &g_JetpackHeightSlider, 750.0f, 10000.0f, "%.1f")) {
            setJetpackHeight(g_JetpackHeightSlider);
        }
    }

    static bool g_FovOverrideEnabled = false;
    if (toggleswap("over-ride fov", g_FovOverrideEnabled)) {
        if (!g_FovOverrideEnabled) {
            setFov(120.0f);
        }
        else {
            fFovValue = getFov();
        }
    }
    tooltips("FOV Changer", "change your fov (supports negative hehe)");

    if (g_FovOverrideEnabled) {
        float sliderWidthFov = ImGui::GetContentRegionAvail().x * 0.5f;
        ImGui::SetNextItemWidth(sliderWidthFov);
        if (ImGui::SliderFloat("##FovSlider", &fFovValue, -175.0f, 175.0f, "player fov: %.1f")) {
            setFov(fFovValue);
        }
    }

    if (toggleswap("no overen*cum*bered", bSprintFatEnabled)) {}
    tooltips("sprint while overweight", "overrides your carryweight so you can sprint while overweight");

    if (bSprintFatEnabled) {
        float sliderWidthCW = ImGui::GetContentRegionAvail().x * 0.5f;
        ImGui::SetNextItemWidth(sliderWidthCW);
        ImGui::SliderFloat("Carryweight", &g_CarryWeightValue, 0.1f, 999.0f, "%.1f");
    }

    static bool g_PlayerFloatEnabled = false;
    if (toggleswap("no gravity", g_PlayerFloatEnabled)) {
        if (g_PlayerFloatEnabled) {
            fVelValue = 0.42f;
            bVelFrozen = true;
            velstate();
        } else {
            bVelFrozen = false;
            velstate();
        }
    }
    tooltips("float!", "locks your velocity");
    SectionHeader("EXTRA");

    if (toggleswap("toggle freecam", bFreecamEnabled)) {
        if (!bFreecamEnabled) {
            bfreecamon = false;
        }
    }
    tooltips("player freecam", "Mouse 5");

    if (bFreecamEnabled) {
        toggleswap("toggle", bfreecamtoggle);
        ImGui::SameLine();
        ImGui::TextDisabled(bfreecamtoggle ? "press" : "hold");
        float freeCamSpeed = getTFC();
        float sliderWidthTFC = ImGui::GetContentRegionAvail().x * 0.5f;
        ImGui::SetNextItemWidth(sliderWidthTFC);
        if (ImGui::SliderFloat("tfc speed", &freeCamSpeed, 0.1f, 1000.0f, "%.2f")) {
            toggleTFC(freeCamSpeed);
        }
        tooltips("free camera speed", "adjusts freecam move speed");
    }

    if (toggleswap("fly", bcharacterflyenabled)) {
        if (!bcharacterflyenabled) setstate(static_cast<std::int32_t>(CharacterStateType::OnGround));
    }
    tooltips("toggle player fly", "makes you fly! woow!");

    if (toggleswap("State modifier", bcharacterstateenabled)) {
        if (!bcharacterstateenabled) setstate(static_cast<std::int32_t>(CharacterStateType::OnGround));
    }

    tooltips("set animaton", "Set character animation state");
    if (bcharacterstateenabled) {
        const char* stateNames[] = { "On Ground", "Jumping", "In Air", "Climbing", "Flying", "Swimming", "Floating" };
        ImGui::SetNextItemWidth(-1);
        ImGui::Combo("##charstate", &g_CharacterStateSelected, stateNames, IM_ARRAYSIZE(stateNames));
    }
    ImGui::Spacing();
}