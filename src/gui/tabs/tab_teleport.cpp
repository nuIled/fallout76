#include "../menu_main/gui.h"
#include "../../core/dll_main/globals.h"
#include "../../../libs/imgui/imgui.h"
#include "../../features/scripts_lader_date/call_function.h"
#include <string>

// i havent touched this tab other than a poor memory read that was conflicting with another function, you can see how aids it is compared to my others i have optimised... thx sussy!

struct PresetTeleport {
    const char* name;
    float x, y, z;
    float pitch, yaw;
    int hotkey;
};

static int g_HotkeyEditIndex = -1;
static bool g_WaitingForHotkey = false;
static bool g_EditingPresetHotkey = false;

static const int NUM_PRESET_TELEPORTS = 6;
static PresetTeleport g_PresetTeleports[NUM_PRESET_TELEPORTS] = {
    {"Gleaming Depths Door", -2328.6189f, 306.1104f, -1736.9559f, 0.0595f, 3.5967f, 0},
    {"Stage 1", 30.0856f, 110.9579f, -91.0880f, 0.7633f, 3.1701f, 0},
    {"Stage 2", 104.1329f, 101.6882f, -99.4612f, -0.0841f, 1.5624f, 0},
    {"Stage 3", 45.5180f, 52.2207f, 29.2668f, 0.6353f, 3.1212f, 0},
    {"Stage 4", -78.1035f, 37.3987f, 14.6273f, 0.7399f, 3.1392f, 0},
    {"Snake Room", -238.418f, 13.9235f, 12.4552f, 0.4575f, 3.6795f, 0}
};

const char* getKeyName(int vkCode) {
    if (vkCode == 0) return "-";
    static char keyName[32];
    
    if (vkCode >= 'A' && vkCode <= 'Z') {
        keyName[0] = (char)vkCode;
        keyName[1] = '\0';
        return keyName;
    }
    if (vkCode >= '0' && vkCode <= '9') {
        keyName[0] = (char)vkCode;
        keyName[1] = '\0';
        return keyName;
    }
    
    if (vkCode >= VK_F1 && vkCode <= VK_F12) {
        snprintf(keyName, sizeof(keyName), "F%d", vkCode - VK_F1 + 1);
        return keyName;
    }
    
    if (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9) {
        snprintf(keyName, sizeof(keyName), "N%d", vkCode - VK_NUMPAD0);
        return keyName;
    }
    
    switch (vkCode) {
        case VK_INSERT: return "Ins";
        case VK_DELETE: return "Del";
        case VK_HOME: return "Hom";
        case VK_END: return "End";
        case VK_PRIOR: return "PgU";
        case VK_NEXT: return "PgD";
        case VK_LEFT: return "<-";
        case VK_RIGHT: return "->";
        case VK_UP: return "Up";
        case VK_DOWN: return "Dn";
        case VK_MULTIPLY: return "N*";
        case VK_ADD: return "N+";
        case VK_SUBTRACT: return "N-";
        case VK_DIVIDE: return "N/";
        case VK_DECIMAL: return "N.";
        case VK_OEM_MINUS: return "-";
        case VK_OEM_PLUS: return "=";
        case VK_OEM_4: return "[";
        case VK_OEM_6: return "]";
        case VK_OEM_1: return ";";
        case VK_OEM_7: return "'";
        case VK_OEM_COMMA: return ",";
        case VK_OEM_PERIOD: return ".";
        case VK_OEM_2: return "/";
        case VK_OEM_5: return "\\";
        case VK_OEM_3: return "`";
        default:
            snprintf(keyName, sizeof(keyName), "%d", vkCode);
            return keyName;
    }
}

const char* getKeyDisplayName(int vkCode) {
    return getKeyName(vkCode);
}

int detectKeyPress() {
    for (int i = 'A'; i <= 'Z'; i++) {
        if (GetAsyncKeyState(i) & 0x8000) return i;
    }
    for (int i = '0'; i <= '9'; i++) {
        if (GetAsyncKeyState(i) & 0x8000) return i;
    }
    for (int i = VK_F1; i <= VK_F12; i++) {
        if (GetAsyncKeyState(i) & 0x8000) return i;
    }
    for (int i = VK_NUMPAD0; i <= VK_NUMPAD9; i++) {
        if (GetAsyncKeyState(i) & 0x8000) return i;
    }
    
    int specialKeys[] = { 
        VK_INSERT, VK_DELETE, VK_HOME, VK_END, VK_PRIOR, VK_NEXT,
        VK_MULTIPLY, VK_ADD, VK_SUBTRACT, VK_DIVIDE, VK_DECIMAL,
        VK_OEM_MINUS, VK_OEM_PLUS, VK_OEM_4, VK_OEM_6, VK_OEM_1,
        VK_OEM_7, VK_OEM_COMMA, VK_OEM_PERIOD, VK_OEM_2, VK_OEM_5, VK_OEM_3 
    };
    for (int key : specialKeys) {
        if (GetAsyncKeyState(key) & 0x8000) return key;
    }
    return 0;
}

static std::string truncateText(const char* text, float maxWidth) {
    ImVec2 textSize = ImGui::CalcTextSize(text);
    if (textSize.x <= maxWidth) {
        return std::string(text);
    }
    
    std::string result(text);
    const char* ellipsis = "..";
    float ellipsisWidth = ImGui::CalcTextSize(ellipsis).x;
    
    while (result.length() > 3 && ImGui::CalcTextSize(result.c_str()).x + ellipsisWidth > maxWidth) {
        result.pop_back();
    }
    
    return result + ellipsis;
}

bool isHotkeyInUse(int vkCode, int skipPresetIndex = -1) {

    for (int i = 0; i < NUM_PRESET_TELEPORTS; i++) {
        if (i != skipPresetIndex && g_PresetTeleports[i].hotkey == vkCode) {
            return true;
        }
    }
    
    return false;
}

void clearConflictingHotkey(int vkCode, int skipPresetIndex = -1) {
    for (int i = 0; i < NUM_PRESET_TELEPORTS; i++) {
        if (i != skipPresetIndex && g_PresetTeleports[i].hotkey == vkCode) {
            g_PresetTeleports[i].hotkey = 0;
        }
    }
}

void checkTPHotkeys() {
    if (g_WaitingForHotkey) return;
    if (bDisplayoverlay) return;

    for (int i = 0; i < NUM_PRESET_TELEPORTS; i++) {
        auto& preset = g_PresetTeleports[i];
        if (preset.hotkey != 0 && (GetAsyncKeyState(preset.hotkey) & 0x8000)) {
            if (writeHavokPosition(preset.x, preset.y, preset.z)) {
                setViewModel(preset.pitch, preset.yaw);
            }
        }
    }

}

void renderTeleportTab() {
    if (g_WaitingForHotkey && g_HotkeyEditIndex >= 0) {
        ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.1f, 1.0f), "Press key (ESC cancel)");

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            g_WaitingForHotkey = false;
            g_HotkeyEditIndex = -1;
            g_EditingPresetHotkey = false;
        }
        else {
            int key = detectKeyPress();
            if (key != 0) {
                int skipPreset = g_EditingPresetHotkey ? g_HotkeyEditIndex : -1;
                if (isHotkeyInUse(key, skipPreset)) {
                    clearConflictingHotkey(key, skipPreset);
                }

                g_PresetTeleports[g_HotkeyEditIndex].hotkey = key;
                g_WaitingForHotkey = false;
                g_HotkeyEditIndex = -1;
                g_EditingPresetHotkey = false;
            }
        }
    }

    ImGui::Spacing();
    const float hotkeyBtnWidth = 46.0f;
    const float clearBtnWidth = 22.0f;

    if (ImGui::BeginTable("##TeleportPresets", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Preset", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, hotkeyBtnWidth);
        ImGui::TableSetupColumn("Clr", ImGuiTableColumnFlags_WidthFixed, clearBtnWidth);
        ImGui::TableHeadersRow();

        for (int i = 0; i < NUM_PRESET_TELEPORTS; i++) {
            auto& preset = g_PresetTeleports[i];
            ImGui::PushID(1000 + i);

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            if (ImGui::SmallButton(preset.name)) {
                writeHavokPosition(preset.x, preset.y, preset.z);
                setViewModel(preset.pitch, preset.yaw);
            }
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("%s", preset.name);
                ImGui::Text("Pos: %.1f, %.1f, %.1f", preset.x, preset.y, preset.z);
                ImGui::EndTooltip();
            }

            ImGui::TableSetColumnIndex(1);
            if (g_WaitingForHotkey && g_EditingPresetHotkey && g_HotkeyEditIndex == i) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.6f, 0.0f, 1.0f));
                ImGui::SmallButton("..");
                ImGui::PopStyleColor();
            } else {
                if (ImGui::SmallButton(getKeyDisplayName(preset.hotkey))) {
                    g_WaitingForHotkey = true;
                    g_HotkeyEditIndex = i;
                    g_EditingPresetHotkey = true;
                }
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set hotkey");

            ImGui::TableSetColumnIndex(2);
            if (ImGui::SmallButton("X")) preset.hotkey = 0;
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Clear hotkey");

            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}