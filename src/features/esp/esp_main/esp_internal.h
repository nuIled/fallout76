#pragma once

#include "esp.h"

namespace ESP {
    extern std::vector<EntityData> g_players;
    extern std::vector<EntityData> g_npcs;
    extern std::vector<EntityData> g_enemies;
    extern std::vector<EntityData> g_entities;
    bool Rpm(std::uintptr_t src, void* dst, size_t size);
    bool IsValidPtr(std::uintptr_t ptr);
    std::uintptr_t GetModuleBase();
    void getScreenSize(float& width, float& height);

    struct Matrix4x4 {
        float m[4][4];
        Matrix4x4();
    };

    Matrix4x4 CreateViewMatrix(const Vec3& pos, float pitchRad, float yawRad);
    Matrix4x4 CreateProjMatrix(float fovDeg, float aspect, float zn, float zf);
    Vec3 NormalizeVec3(const Vec3& v);
    bool getViewAngles(float& pitch, float& yaw);
    Vec3 getLocPlayerXYZ();
    Vec2 W2S(const Vec3& pos, float screenW, float screenH, bool& visible);
    EntityCategory Classify(const TESItem& base);
    ActorState getAS(const TESObjectRefr& refr);
    void readAS(const TESObjectRefr& refr, EntityData& out);
    TESObjectRefr readLocPlayer(std::uintptr_t& outPtr);
    LoadedAreaManager readLAM();
    std::string readEntName(std::uintptr_t namePtr);
    std::string getBaseObjName(const TESItem& item);
    std::vector<TESObjectCell> getLoadedCells();
    std::vector<std::pair<std::uintptr_t, TESObjectRefr>> getCellObj(const TESObjectCell& cell);
    std::uintptr_t GetPtrFromFormId(std::uint32_t formId);
    std::string readPlayerName(const ClientAccount& account);
}