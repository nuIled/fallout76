#include "sig_resolver.h"
#include "../../../core/sig_main/sig.h"
#include "../../../core/dll_main/globals.h"

static uintptr_t s_local_player = 0;
static uintptr_t s_havok_physics = 0;
static uintptr_t s_player_speed = 0;
static uintptr_t s_player_size = 0;
static uintptr_t s_player_camera = 0;
static uintptr_t s_tfc = 0;
static uintptr_t s_tfc_speed = 0;
static uintptr_t s_loaded_area_manager = 0;
static bool s_resolved = false;

static void resolvePointers() {
    if (s_resolved) return;
    s_resolved = true;
    s_local_player = sigMain("48 8D 05 ? ? ? ? 0F 57 C0 48 89 05 ? ? ? ? 48 8D 0D", 0x2A, 0, 0);
    s_havok_physics = sigMain("48 89 44 24 38 48 8D 1D ? ? ? ? 48 89 5C 24 40 44 8B 05", 8, 0, 0x10);
    s_player_speed = sigMain("33 D2 E8 ? ? ? ? 48 89 05 ? ? ? ? 48 85 C0 75 71 8D 4B 68", 0xA, 0, 0);
    s_player_size = sigMain("89 05 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 48 83 3D ? ? ? ? 00 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 05", 0x27, 0, 0x100);
    s_player_camera = sigMain("48 8B 0D ? ? ? ? 25", 3, 0, 0);
    s_tfc = sigMain("48 89 5C 24 ? 57 48 83 EC ? 48 8B D9 0F B6 FA 48 8B 49 ? 48 85 C9 74 ? 48 3B 8B", 0, -1, 0); // untested - works
    s_tfc_speed = sigMain("F3 0F 10 05 ? ? ? ? 0F 2F C6 0F 83", 4, 0, 0);
    s_loaded_area_manager = sigMain("48 8B 0D ? ? ? ? F6 40", 3, 0, 0); // untested - works
}

uintptr_t getlocal_player() {
    resolvePointers();
    if (s_local_player) return s_local_player;
    return getbase() + local_player;
}

uintptr_t gethavok_physics() {
    resolvePointers();
    if (s_havok_physics) return s_havok_physics;
    return getbase() + havok_physics;
}

uintptr_t getplayer_speed() {
    resolvePointers();
    if (s_player_speed) return s_player_speed;
    return getbase() + player_speed;
}

uintptr_t getplayer_size() {
    resolvePointers();
    if (s_player_size) return s_player_size;
    return getbase() + player_size;
}

uintptr_t get_player_camera() {
    resolvePointers();
    return s_player_camera;
}

uintptr_t get_tfc() {
    resolvePointers();
    return s_tfc;
}

uintptr_t get_tfc_speed() {
    resolvePointers();
    return s_tfc_speed;
}

uintptr_t get_loaded_area_manager() {
    resolvePointers();
    if (s_loaded_area_manager) return s_loaded_area_manager;
    return getbase() + loaded_area_manager;
}