#include "../../core/dll_main/globals.h"
#include "../../../libs/imgui/imgui.h"
#include "gui.h"
#include <vector>
#include <cstdio>

void tooltips(const char* title, const char* description) {
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 6));
        ImGui::BeginTooltip();
        ImVec4 accentColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
        ImGui::PushStyleColor(ImGuiCol_Text, accentColor);
        ImGui::TextUnformatted(title);
        ImGui::PopStyleColor();
        ImGui::Separator();
        ImGui::PushTextWrapPos(250.0f);
        ImGui::TextUnformatted(description);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
        ImGui::PopStyleVar();
    }
}

bool bnoclipenabled = false;
bool bnocliptoggle = false;
bool bnoclipon = false;
float fNoclipSpeedMult = 0.00495f;
bool bfreecamtoggle = false;
bool bfreecamon = false;
bool bMainMenuVisible = false;
int bSelectedTab = 0;

void renderMenu() {
    checkTPHotkeys();
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
    }
    static bool themeApplied = false;
    if (!themeApplied) {
        menuTheme(bdarkmodeenabled, 1.0f);
        themeApplied = true;
    }
    
    bMainMenuVisible = false;
    if (bDisplayoverlay) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 windowSize = ImVec2(820.0f, 740.0f);
        ImVec2 centerPos = ImVec2(
            (viewport->Size.x - windowSize.x) * 0.5f,
            (viewport->Size.y - windowSize.y) * 0.5f
        );
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
        ImGui::SetNextWindowPos(centerPos, ImGuiCond_Always);
        
        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize;
        
        if (ImGui::Begin("admin", nullptr, windowFlags)) {
            bMainMenuVisible = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
            const char* tabNames[] = { "self", "combat", "misc", "teleporter", "ESP", "admin" };
            if (ImGui::BeginTabBar("##MainTabs")) {
                for (int i = 0; i < IM_ARRAYSIZE(tabNames); i++) {
                    const bool opened = ImGui::BeginTabItem(tabNames[i]);
                    if (opened) {
                        bSelectedTab = i;
                        switch (bSelectedTab) {
                            case 0: renderMainTab(); break;
                            case 1: renderWepTab(); break;
                            case 2: renderMiscTab(); break;
                            case 3: renderTeleportTab(); break;
                            case 4: renderESPTab(); break;
                            case 5: renderAdminTab(); break;
                        }
                        ImGui::EndTabItem();
                    }
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }
    if (!bDisplayoverlay) {
        ImGui::SetNextWindowPos(ImVec2(15, 15), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.0f);
        
        ImGuiWindowFlags miniFlags = 
            ImGuiWindowFlags_NoCollapse | 
            ImGuiWindowFlags_NoTitleBar | 
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoBackground;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14, 12));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
        
        if (ImGui::Begin("##MiniOverlay", nullptr, miniFlags)) {
            ImGui::TextUnformatted("i am not a cheater!");
            ImGui::Spacing(); 
            ImGui::TextUnformatted("ilydealer");
        }
        ImGui::End();
        ImGui::PopStyleVar(2);
    }
}