#include "../esp_main/esp_internal.h"
#include "imgui/imgui.h"
#include "../../../core/dll_main/globals.h"
#include <Windows.h>
#include <cmath>
#include <cstring>

namespace ESP {
    constexpr float ESP_PI = 3.14159265358979323846f;

    static float DegToRad(float degrees) {
        return degrees * (ESP_PI / 180.0f);
    }

    static Vec3 Cross(const Vec3& a, const Vec3& b) {
        return Vec3{
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    static float Dot(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    bool Rpm(std::uintptr_t src, void* dst, size_t size) {
        __try {
            std::memcpy(dst, reinterpret_cast<const void*>(src), size);
            return true;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    bool IsValidPtr(std::uintptr_t ptr) {
        return ptr >= 0x10000 && ptr < 0x7FFFFFFFFFFF;
    }

    std::uintptr_t GetModuleBase() {
        return getbase();
    }

    void getScreenSize(float& width, float& height) {
        ImGuiIO& io = ImGui::GetIO();
        if (io.DisplaySize.x > 0.0f && io.DisplaySize.y > 0.0f) {
            width = io.DisplaySize.x;
            height = io.DisplaySize.y;
            return;
        }
        width = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
        height = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));
        if (width <= 0.0f || height <= 0.0f) {
            width = 1920.0f;
            height = 1080.0f;
        }
    }

    Vec3 NormalizeVec3(const Vec3& v) {
        float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        if (len > 0.0001f)
            return Vec3{ v.x / len, v.y / len, v.z / len };
        return Vec3{ 0.0f, 0.0f, 0.0f };
    }

    Matrix4x4::Matrix4x4() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    Matrix4x4 CreateViewMatrix(const Vec3& pos, float pitchRad, float yawRad) {
        Vec3 forward;
        float adjustedYaw = -yawRad;
        float adjustedPitch = -pitchRad;
        forward.x = std::cosf(adjustedPitch) * std::cosf(adjustedYaw);
        forward.y = std::sinf(adjustedPitch);
        forward.z = std::cosf(adjustedPitch) * std::sinf(adjustedYaw);
        forward = NormalizeVec3(forward);
        Vec3 worldUp{ 0.0f, 1.0f, 0.0f };
        Vec3 right = NormalizeVec3(Cross(worldUp, forward));
        Vec3 up = Cross(forward, right);
        Matrix4x4 view;
        view.m[0][0] = right.x; view.m[0][1] = up.x; view.m[0][2] = forward.x; view.m[0][3] = 0.0f;
        view.m[1][0] = right.y; view.m[1][1] = up.y; view.m[1][2] = forward.y; view.m[1][3] = 0.0f;
        view.m[2][0] = right.z; view.m[2][1] = up.z; view.m[2][2] = forward.z; view.m[2][3] = 0.0f;
        view.m[3][0] = -Dot(right, pos); view.m[3][1] = -Dot(up, pos); view.m[3][2] = -Dot(forward, pos); view.m[3][3] = 1.0f;
        return view;
    }

    Matrix4x4 CreateProjMatrix(float fovDeg, float aspect, float zn, float zf) {
        float fovRad = DegToRad(fovDeg);
        float yScale = 1.0f / std::tanf(fovRad * 0.5f);
        float xScale = yScale / aspect;
        Matrix4x4 proj;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                proj.m[i][j] = 0.0f;
        proj.m[0][0] = xScale;
        proj.m[1][1] = yScale;
        proj.m[2][2] = zf / (zf - zn);
        proj.m[2][3] = 1.0f;
        proj.m[3][2] = (-zn * zf) / (zf - zn);
        proj.m[3][3] = 0.0f;
        return proj;
    }
}