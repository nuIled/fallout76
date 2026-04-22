#include "esp.h"
#include "esp_internal.h"
#include "../../../core/dll_main/globals.h"
#include <cmath>

namespace ESP {
    std::vector<EntityData> g_players;
    std::vector<EntityData> g_npcs;
    std::vector<EntityData> g_enemies;
    std::vector<EntityData> g_entities;
    std::vector<EntityData> GetPlayers() {
        return g_players;
    }

    std::vector<EntityData> GetNPCs() {
        return g_npcs;
    }

    ESPDebugInfo GetDebugInfo() {
        ESPDebugInfo info;
        info.enabled = Config::enable;
        Vec3 localPos = getLocPlayerXYZ();
        float pitch = 0.0f, yaw = 0.0f;
        bool hasAngles = getViewAngles(pitch, yaw);
        info.cameraValid = (localPos.x != 0.0f || localPos.y != 0.0f || localPos.z != 0.0f) && hasAngles;
        info.localPlayerPos = localPos;
        info.playerCount = static_cast<int>(g_players.size());
        info.npcCount = static_cast<int>(g_npcs.size());
        float screenW = 1920.0f;//maybe this is abad idea lolol
        float screenH = 1080.0f;
        getScreenSize(screenW, screenH);
        info.screenWidth = screenW;
        info.screenHeight = screenH;

        if (info.cameraValid) {
            info.cameraOrigin = localPos;
            info.cameraForward.x = std::cosf(pitch) * std::cosf(yaw);
            info.cameraForward.y = -std::sinf(pitch);
            info.cameraForward.z = std::cosf(pitch) * std::sinf(yaw);
            info.cameraForward = NormalizeVec3(info.cameraForward);
            Matrix4x4 view = CreateViewMatrix(localPos, pitch, yaw);
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    info.viewMatrix[i][j] = view.m[i][j];
                }
            }
            auto base = GetModuleBase();
            if (base) {
                std::uintptr_t localPlayerPtr = 0;
                if (Rpm(getlocal_player(), &localPlayerPtr, sizeof localPlayerPtr)) {
                    info.cameraPtr = localPlayerPtr;
                }
            }
        }
        return info;
    }
}