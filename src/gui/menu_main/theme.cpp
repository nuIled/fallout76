#include "gui.h"
#include "../../../libs/imgui/imgui.h"

bool bdarkmodeenabled = true;

void menuTheme(bool darkMode, float alpha) {
    if (darkMode) {
        ImGui::StyleColorsDark();
    } else {
        ImGui::StyleColorsClassic();
    }

    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = alpha;
}