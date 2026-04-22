#include "../menu_main/gui.h"
#include "../../core/dll_main/globals.h"
#include "../../../libs/imgui/imgui.h"
#include <vector>
#include <windows.h>

float ftrans = 1.0f;
float fbgalpha = 1.0f;
float fwindowbordersize = 1.0f;
float fchildbordersize = 1.0f;
float ffontsize = 20.0f;

void renderCfgTab() {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    ImGui::Text("Settings");
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();
}

// once upon a time there was something here...