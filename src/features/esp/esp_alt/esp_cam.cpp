#include "../esp_main/esp_internal.h"
#include "../../../core/dll_main/globals.h"
#include <cmath>
#include <cstring>
/* the entire esp is pasted as fuck, i failed maths :p */
namespace ESP {
namespace {
bool GameViewMatrixLooksValid(const Matrix4x4& v) {
    float sum = 0.0f;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float x = v.m[i][j];
            if (!std::isfinite(x))
                return false;
            sum += std::fabs(x);
        }
    }
    return sum > 1.0e-4f;
}

void MulHomogeneousWorldByView(const Matrix4x4& view, float wx, float wy, float wz,
    float& outX, float& outY, float& outZ, float& outW) {
    outX = wx * view.m[0][0] + wy * view.m[1][0] + wz * view.m[2][0] + view.m[3][0];
    outY = wx * view.m[0][1] + wy * view.m[1][1] + wz * view.m[2][1] + view.m[3][1];
    outZ = wx * view.m[0][2] + wy * view.m[1][2] + wz * view.m[2][2] + view.m[3][2];
    outW = wx * view.m[0][3] + wy * view.m[1][3] + wz * view.m[2][3] + view.m[3][3];
}

Vec2 ClipToScreen(float clipX, float clipY, float clipZ, float clipW,
    float screenW, float screenH, bool& visible) {
    Vec2 result{};
    visible = false;
    if (clipW <= 0.01f)
        return result;
    float ndcX = clipX / clipW;
    float ndcY = clipY / clipW;
    float ndcZ = clipZ / clipW;
    if (ndcZ < 0.0f || ndcZ > 1.0f)
        return result;
    if (ndcX < -1.0f || ndcX > 1.0f || ndcY < -1.0f || ndcY > 1.0f)
        return result;
    result.x = (ndcX + 1.0f) * 0.5f * screenW;
    result.y = (1.0f - ndcY) * 0.5f * screenH;
    result.x = screenW - result.x;
    visible = true;
    return result;
}

}
    bool readCam(Camera& out) {
        auto base = GetModuleBase();
        if (!base) return false;
        std::uintptr_t camPtr = 0;
        if (!Rpm(base + esp_camera, &camPtr, sizeof camPtr))
            return false;
        if (!IsValidPtr(camPtr))
            return false;
        if (!Rpm(camPtr, &out, sizeof out))
            return false;
        return true;
    }

    bool getViewAngles(float& pitch, float& yaw) {
        auto base = GetModuleBase();
        if (!base) return false;
        std::uintptr_t localPlayerPtr = 0;
        if (!Rpm(getlocal_player(), &localPlayerPtr, sizeof localPlayerPtr))
            return false;
        if (!IsValidPtr(localPlayerPtr))
            return false;
        if (!Rpm(localPlayerPtr + 0x60, &pitch, sizeof pitch))
            return false;
        if (!Rpm(localPlayerPtr + 0x68, &yaw, sizeof yaw))
            return false;
        return true;
    }

    Vec3 getLocPlayerXYZ() {
        auto base = GetModuleBase();
        if (!base) return Vec3{};
        std::uintptr_t localPlayerPtr = 0;
        if (!Rpm(getlocal_player(), &localPlayerPtr, sizeof localPlayerPtr))
            return Vec3{};
        if (!IsValidPtr(localPlayerPtr))
            return Vec3{};
        Vec3 pos{};
        if (!Rpm(localPlayerPtr + 0xAD8, &pos.x, sizeof pos.x))
            return Vec3{};
        if (!Rpm(localPlayerPtr + 0xADC, &pos.y, sizeof pos.y))
            return Vec3{};
        if (!Rpm(localPlayerPtr + 0xAE0, &pos.z, sizeof pos.z))
            return Vec3{};
        return pos;
    }

    Vec2 W2S(const Vec3& world, float screenW, float screenH, bool& visible) {
        Vec2 result{};
        visible = false;

        float aspect = (screenH > 0.0f) ? (screenW / screenH) : 1.77f;
        float fov = Config::fov > 0.0f ? Config::fov : 90.0f;
        Matrix4x4 proj = CreateProjMatrix(fov, aspect, 0.1f, 10000.0f);

        float viewX = 0.0f;
        float viewY = 0.0f;
        float viewZ = 0.0f;
        float viewW = 0.0f;

        Camera cam{};
        bool usedGameView = false;
        if (readCam(cam)) {
            Matrix4x4 gameView{};
            std::memcpy(gameView.m, cam.viewMatrix, sizeof(gameView.m));
            if (GameViewMatrixLooksValid(gameView)) {
                MulHomogeneousWorldByView(gameView, world.x, world.y, world.z, viewX, viewY, viewZ, viewW);
                usedGameView = true;
            }
        }

        if (!usedGameView) {
            Vec3 cameraPos = getLocPlayerXYZ();
            float pitch = 0.0f, yaw = 0.0f;
            if (!getViewAngles(pitch, yaw))
                return result;
            yaw = -yaw;
            Vec3 adjustedCameraPos = cameraPos;
            float tempXY = adjustedCameraPos.x;
            adjustedCameraPos.x = adjustedCameraPos.y;
            adjustedCameraPos.y = tempXY;
            float tempYZ = adjustedCameraPos.y;
            adjustedCameraPos.y = adjustedCameraPos.z;
            adjustedCameraPos.z = tempYZ;
            Matrix4x4 view = CreateViewMatrix(adjustedCameraPos, pitch, yaw);
            Vec3 adjustedWorld = world;
            tempXY = adjustedWorld.x;
            adjustedWorld.x = adjustedWorld.y;
            adjustedWorld.y = tempXY;
            tempYZ = adjustedWorld.y;
            adjustedWorld.y = adjustedWorld.z;
            adjustedWorld.z = tempYZ;
            MulHomogeneousWorldByView(view, adjustedWorld.x, adjustedWorld.y, adjustedWorld.z, viewX, viewY, viewZ, viewW);
        }

        float clipX = viewX * proj.m[0][0] + viewY * proj.m[1][0] + viewZ * proj.m[2][0] + viewW * proj.m[3][0];
        float clipY = viewX * proj.m[0][1] + viewY * proj.m[1][1] + viewZ * proj.m[2][1] + viewW * proj.m[3][1];
        float clipZ = viewX * proj.m[0][2] + viewY * proj.m[1][2] + viewZ * proj.m[2][2] + viewW * proj.m[3][2];
        float clipW = viewX * proj.m[0][3] + viewY * proj.m[1][3] + viewZ * proj.m[2][3] + viewW * proj.m[3][3];

        return ClipToScreen(clipX, clipY, clipZ, clipW, screenW, screenH, visible);
    }

    Vec3 getLocPlayerXYZpos() {
        return getLocPlayerXYZ();
    }
}