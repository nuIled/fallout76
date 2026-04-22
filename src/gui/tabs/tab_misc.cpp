#include "../menu_main/gui.h"
#include "../../core/dll_main/globals.h"
#include "../../../libs/imgui/imgui.h"

static void MiscSectionHeader(const char* label) {
    ImGui::TextUnformatted(label);
    ImGui::Separator();
    ImGui::Spacing();
}

void renderMiscTab() {
    ImGui::Spacing();
    MiscSectionHeader("WORLD");

    static bool bWeatherOverrideEnabled = false;
    if (toggleswap("time of day", bWeatherOverrideEnabled)) {
        if (!bWeatherOverrideEnabled) {
            setWeather(12.0f);
        } else {
            fWeatherValue = getWeather();
        }
    }
    tooltips("time Of day", "Override the time of day, bypasses set weather (custom camp weathers)");

    if (bWeatherOverrideEnabled) {
        float sliderWidthWeather = ImGui::GetContentRegionAvail().x * 0.5f;
        ImGui::SetNextItemWidth(sliderWidthWeather);
        if (ImGui::SliderFloat("##TimeOfDaySlider", &fWeatherValue, 0.0f, 24.0f, "time of day: %.1f")) {
            setWeather(fWeatherValue);
        }
    }

    static bool bCloudSpeedOverride = false;
    if (toggleswap("star posiiton", bCloudSpeedOverride)) {
        if (!bCloudSpeedOverride) {
            setCloudSpeed(0.10002f);
        } else {
            fCloudSpeed = getCloudSpeed();
        }
    }
    tooltips("star position", "Override star position");

    if (bCloudSpeedOverride) {
        float sliderWidthClouds = ImGui::GetContentRegionAvail().x * 0.5f;
        ImGui::SetNextItemWidth(sliderWidthClouds);
        if (ImGui::SliderFloat("##CloudSpeedSlider", &fCloudSpeed, 0.5f, 100.0f, "cloud speed: %.1f")) {
            setCloudSpeed(fCloudSpeed);
        }
    }

    //static bool bStarPosiitonOverride = false;
    //if (toggleswap("Change Star Position", bStarPosiitonOverride)) {
    //    if (!bStarPosiitonOverride) {
    //        setStarPos(0.10002f);
    //    } else {
    //        fStarPos = getStarPos();
    //    }
    //}
    //tooltips("Change Posiiton of Stars", "Override Stars");

    MiscSectionHeader("SIZE EDITOR");

    if (!fcharsizeloaded) {
        getPlayerSize(fcharsigma, fcharfat, fcharthin);
        fcharsizeloaded = true;
    }

    static bool g_SkinnyPresetActive = false;
    static bool g_FatPresetActive = false;
    static bool g_DebugPresetActive = false;
    static double g_LastPresetWriteTime = 0.0;
    double currentTime = ImGui::GetTime();

    if (toggleswap("skinny", g_SkinnyPresetActive)) {
        if (g_SkinnyPresetActive) {
            g_FatPresetActive = false;
            g_DebugPresetActive = false;
            fcharsigma = 0.000f;
            fcharfat = 0.000f;
            fcharthin = 0.511f;
            setPlayerSize(fcharsigma, fcharfat, fcharthin);
            g_LastPresetWriteTime = currentTime;
        }
    }
    tooltips("skinny", "0, 0, 0.511");

    if (toggleswap("fat", g_FatPresetActive)) {
        if (g_FatPresetActive) {
            g_SkinnyPresetActive = false;
            g_DebugPresetActive = false;
            fcharsigma = 1.250f;
            fcharfat = 1.250f;
            fcharthin = 1.250f;
            setPlayerSize(fcharsigma, fcharfat, fcharthin);
            g_LastPresetWriteTime = currentTime;
        }
    }
    tooltips("fat", "1.25, 1.25, 1.25");

    if (toggleswap("debug", g_DebugPresetActive)) {
        if (g_DebugPresetActive) {
            g_SkinnyPresetActive = false;
            g_FatPresetActive = false;
            fcharsigma = 0.000f;
            fcharfat = 0.000f;
            fcharthin = 0.000f;
            setPlayerSize(fcharsigma, fcharfat, fcharthin);
        }
    }
    tooltips("debug", "0.000, 0.000, 0.000");

    static bool g_CustomPresetActive = false;
    toggleswap("custom", g_CustomPresetActive);
    tooltips("custom", "customise the values for size");

    if (g_CustomPresetActive) {
        ImGui::Spacing();
        float sliderWidth = ImGui::GetContentRegionAvail().x * 0.5f;
        ImGui::TextUnformatted("Muscular:");
        ImGui::SetNextItemWidth(sliderWidth);
        ImGui::SliderFloat("##muscular", &fcharsigma, 0.0f, 2.0f, "%.3f");
        ImGui::TextUnformatted("Fat:");
        ImGui::SetNextItemWidth(sliderWidth);
        ImGui::SliderFloat("##large", &fcharfat, 0.0f, 2.0f, "%.3f");
        ImGui::TextUnformatted("Skinny:");
        ImGui::SetNextItemWidth(sliderWidth);
        ImGui::SliderFloat("##thin", &fcharthin, 0.0f, 2.0f, "%.3f");
        ImGui::Spacing();
    }

    ImVec2 applyTextSize = ImGui::CalcTextSize("apply");
    if (ImGui::Button("apply", ImVec2(applyTextSize.x + ImGui::GetStyle().FramePadding.x * 2, 0))) {
        setPlayerSize(fcharsigma, fcharfat, fcharthin);
    }

    if (g_SkinnyPresetActive || g_FatPresetActive) {
        if (currentTime - g_LastPresetWriteTime >= 0.5) {
            if (g_SkinnyPresetActive) {
                fcharsigma = 0.000f;
                fcharfat = 0.000f;
                fcharthin = 0.511f;
            } else if (g_FatPresetActive) {
                fcharsigma = 1.250f;
                fcharfat = 1.250f;
                fcharthin = 1.250f;
            }
            setPlayerSize(fcharsigma, fcharfat, fcharthin);
            g_LastPresetWriteTime = currentTime;
        }
    }

    ImGui::Spacing();
    MiscSectionHeader("tools / debugging");

    if (ImGui::Button("53354363456345")) {
        dumpenums();
    }
    tooltips("Dump enum data", "87593875938");
}

