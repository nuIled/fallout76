#include "../menu_main/gui.h"
#include "../../core/dll_main/globals.h"
#include "../../../libs/imgui/imgui.h"

static void WepSectionHeader(const char* label) {
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

void renderWepTab() {
    WepSectionHeader("weapons / combat");
    ImGui::Spacing();
    if (toggleswap("always crit", bCritsEnabled)) {
        if (!bCritsEnabled) getcrits(0.0f);
    }
    tooltips("always crit", "all shots will be criticals");

    if (toggleswap("all ranged auto", bAmmoEnabled)) {
        if (!bAmmoEnabled) {
            bAmmoActive = false;
            setAmmoValue(0x0037D0C1);
        } else {
            if (bcustomOmodEnabled) bcustomOmodEnabled = false;
            bAmmoActive = true;
            bcustomOmodEnabled = false;
            setAmmoValue(0x00113689);
        }
    }
    tooltips("deprecated infinite ammo", "old infinite ammo, only works onm weapons with default appearence, but ideal to use only for making weapons auto");
    ImGui::Spacing();
    if (toggleswap("all melee auto", bMeleeAutoEnabled)) {
        if (!bMeleeAutoEnabled)
            setmeleeautoval(0x005117BF);
        else
            setmeleeautoval(0x001F660D);
    }
    tooltips("all melee auto", "only works on specific weapons, ideally, use the melee speed ba2 with this!");
    ImGui::Spacing();
    if (toggleswap("weapon customiser", bcustomOmodEnabled)) { // my spelling is superior muahhaha
        if (!bcustomOmodEnabled) {
            bAmmoActive = false;
            setAmmoValue(0x0037D0C1);
        } else {
            if (bAmmoEnabled) {
                bAmmoEnabled = false;
                bAmmoActive = false;
                setAmmoValue(0x0037D0C1);
            }
            if (!bCustomOmodValApply) {
                uiCustomOmodVal = 0x00113689;
                bCustomOmodValApply = true;
            }
            setAmmoValue(uiCustomOmodVal);
        }
    }
    tooltips("customise OMOD's", "apply custom omods to weapons (swaps default appearence)");

    if (bcustomOmodEnabled) {
        ImGui::TextWrapped("");
        ImGui::Text("Form ID:");
        ImGui::SetNextItemWidth(120);
        ImGui::InputText("##CustomOmodInput_wep", cCustomOmodInput, sizeof(cCustomOmodInput),
                         ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
        ImGui::SameLine();
        if (ImGui::Button("Apply##CustomOmod_wep")) {
            char* endPtr;
            unsigned long parsedValue = strtoul(cCustomOmodInput, &endPtr, 16);
            if (endPtr != cCustomOmodInput && *endPtr == '\0' && parsedValue <= 0xFFFFFFFF) {
                uiCustomOmodVal = static_cast<uint32_t>(parsedValue);
                bCustomOmodValApply = true;
                if (bcustomOmodEnabled) setAmmoValue(uiCustomOmodVal);
            }
        }
    }
    ImGui::Spacing();
    static bool bMeleeSpeedEnabled = false;
    if (toggleswap("melee speed", bMeleeSpeedEnabled)) {
        if (!bMeleeSpeedEnabled) {
            setMeleeSpeed(100.0f);
        }
        else {
            fMeleeSpeed = getMeleeSpeed();
        }
    }
    tooltips("modify 2 handed melee speed", "manually set your 2 handed melee speed to whatever the fuck you want! ");

    if (bMeleeSpeedEnabled) {
        float sliderMeleeSpeed = ImGui::GetContentRegionAvail().x * 0.5f;
        ImGui::SetNextItemWidth(sliderMeleeSpeed);
        if (ImGui::SliderFloat("##MeleeSpeedSlider", &fMeleeSpeed, 0.1f, 100.0f, "melee speed: %.1f")) {
            setMeleeSpeed(fMeleeSpeed);
        }
    }

    ImGui::Spacing();
    toggleswap("infinite ammo", bWepInfAmmo);
    tooltips("infinite ammo", "remove the need for ammo!");
    toggleswap("no reload", bWepNoReload);
    tooltips("No reload", "disable reloading");
    toggleswap("remove ap cost", bWepNoAP);
    tooltips("no weapon AP cost", "remove ap cost from weapons");
    toggleswap("no attack delay", bWepNoAttackDelay);
    tooltips("remove  attack delay", "remove attack delay");
    toggleswap("customise firerate", bWepFFREnable);
    tooltips("modify your firerate", "override firerate with slider below");
    if (bWepFFREnable) {
        float sliderWidthFR = ImGui::GetContentRegionAvail().x * 0.5f;
        ImGui::SetNextItemWidth(sliderWidthFR);
        ImGui::SliderFloat("##firerate", &fWepFFR, 1.0f, 100.0f, "%.0f");
    }
    if (ImGui::Button("apply firerate")) {
        applyMods();
    }
    tooltips("apply firerate", "resolves weapon data via ESM");
}