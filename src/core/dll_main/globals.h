#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <cstdint>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <atomic>
#include <iostream>
#include <dwmapi.h>
#include <tchar.h>
#include <vector>
#include <cmath>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// player_size = "89 05 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 48 83 3D ? ? ? ? 00 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 05"_sig.at(0x27).rip().at(0x100).as<AuMach&>();
// loaded_area_manager = "48 8D 55 30 48 8B 0D ? ? ? ? E8 ? ? ? ? 83 F8 01 75 1F"_sig.at(7).rip().as<LoadedAreaManager**>();
// appMain = "48 8B 05 ? ? ? ? 48 8B 58 ? FF 15"_sig.at(3).rip();
// local_player = "48 8D 05 ? ? ? ? 0F 57 C0 48 89 05 ? ? ? ? 48 8D 0D"_sig.at(0x2A).rip();
// havok_physics = "48 89 44 24 38 48 8D 1D ? ? ? ? 48 89 5C 24 40 44 8B 05"_sig.at(8).rip().at(0x10);
// player_speed = "33 D2 E8 ? ? ? ? 48 89 05 ? ? ? ? 48 85 C0 75 71 8D 4B 68"_sig.at(0xA).rip();

namespace Offsets { // ommmgmgmgmg aaaaaa offsets bafkm adhakwjdh
    constexpr uintptr_t havok_physics = 0x0629A6A0;
    constexpr uintptr_t local_player = 0x06051E78;
    constexpr uintptr_t player_size = 0x05A00D88; //wrong
    constexpr uintptr_t player_speed = 0x05F6BCD8;
    constexpr uintptr_t jetpack_height = 0x05A006A8; // fJetpackThrustInitial
    constexpr uintptr_t superjump = 0x05A31FA8; // fJumpHeightMin
    constexpr uintptr_t esm_base = 0x05E27340; // just a refr, use the sig
    constexpr uintptr_t esm_pool_offset = 0x70;
    constexpr uintptr_t loaded_area_manager = 0x058BD880;
    constexpr uintptr_t esp_main = 0x060AD168;
    constexpr uintptr_t esp_camera = 0x0704B898;
    constexpr uintptr_t omod = 0x05E5F490;
    constexpr uintptr_t weather = 0x054B2C18; // fWeatherHourOverride
    constexpr uintptr_t melee = 0x054C8408; // fWeaponTwoHandedAnimationSpeedMult
    constexpr uintptr_t cloud = 0x054B2530; // fWeatherCloudSpeedMax
    constexpr uintptr_t stars = 0x054B2C38; // fStarsRotateDays
    constexpr uintptr_t fov = 0x05AD8908; // Fallout76.exe+5AD8908
}// PLEASE DEALER PLEASE OFFSETS FOR THE LATEST PATCH PLEASE MY FAMILY IS STARVING

// thrust initial 0x059FF6B0 for jet boost
// thrust sustained 0x059FF6D0
// drain initial 0x059FF6F0 -> works similar to tr? - no, this is main
// drain sustained 0x059FF710

constexpr uintptr_t havok_physics = Offsets::havok_physics;
constexpr uintptr_t local_player = Offsets::local_player;
constexpr uintptr_t player_size = Offsets::player_size;
constexpr uintptr_t player_speed = Offsets::player_speed;
constexpr uintptr_t jetpack_height = Offsets::jetpack_height;
constexpr uintptr_t superjump = Offsets::superjump;
constexpr uintptr_t esm_base = Offsets::esm_base;
constexpr uintptr_t esm_pool_offset = Offsets::esm_pool_offset;
constexpr uintptr_t loaded_area_manager = Offsets::loaded_area_manager;
constexpr uintptr_t esp_main = Offsets::esp_main;
constexpr uintptr_t esp_camera = Offsets::esp_camera;
constexpr uintptr_t omod = Offsets::omod;
constexpr uintptr_t weather = Offsets::weather;
constexpr uintptr_t melee = Offsets::melee;
constexpr uintptr_t cloud = Offsets::cloud;
constexpr uintptr_t stars = Offsets::stars;
constexpr uintptr_t fov = Offsets::fov;

struct __declspec(align(8)) FORM_ENUM_STRING {
    BYTE cFormID;
    const char* pFormString;
    unsigned int iFormString;
};

extern FORM_ENUM_STRING* g_FormEnumString;
extern void dumpenums();//scar
extern uintptr_t getbase();

#include "../../features/memory/owo_whats_this/sig_resolver.h"

extern void loadD3D11Functions();
extern void hookSwapChain(IDXGISwapChain* pSwapChain);
extern bool bCritsDisabled;
extern bool bCritsEnabled;
extern void setcrits();
extern void getcrits(float value);
extern void applyMods();
extern void runesmpatcher();
extern bool bWepInfAmmo;
extern bool bWepNoReload;
extern bool bWepNoAP;
extern bool bWepFFREnable;
extern bool bWepNoAttackDelay;
extern bool bWepModsCont;
extern float fWepFFR;
extern bool bCustomOmodValApply;
extern uint32_t uiCustomOmodVal;
extern bool bcustomOmodEnabled;
extern char cCustomOmodInput[16];
extern bool bNoclipEnabled;
extern bool bnoclipenabled;
extern bool bnocliptoggle;
extern bool bnoclipon;
extern float fNoclipSpeedMult;
extern void noclipLoop();
extern float fCurViewPitch;
extern float fCurViewYaw;
extern float fNoclipSpeed;
extern uintptr_t uiViewAngleOffset;
extern void cacheVA();
extern bool bVelDisabled;
extern bool bVelEnabled;
extern float fVelValue;
extern bool bVelFrozen;
extern bool bVelNoclipFrozen;
extern void applyVel();
extern void getVel(float value);
extern float getVelocity();
extern void velstate();
extern void getPlayerSize(float& muscular, float& large, float& thin);
extern void setPlayerSize(float muscular, float large, float thin);
extern bool getWorldCoords(float& x, float& y, float& z);
extern bool getHavokPosition(float& x, float& y, float& z);
extern bool writeHavokPosition(float x, float y, float z);
extern bool getViewModel(float& pitch, float& yaw);
extern bool setViewModel(float pitch, float yaw);
extern bool playerSpeedboostStart;
extern bool playerSpeedboostStop;
extern void setPlayerSpeed();
extern void getSpeed(float value);
extern bool bAmmoDisabled;
extern bool bAmmoEnabled;
extern bool bAmmoActive;
extern bool bAmmoStop;
extern void runomodpatcher();
extern void setAmmoValue(uint32_t value);
extern void setmeleeautoval(uint32_t value);
extern bool bMeleeAutoEnabled;
extern bool bSprintFatEnabled;
extern bool bSprintFatDisabled;
extern float g_CarryWeightValue;
extern void runSprintOverweightLoop();
extern void setSuperJump(float value);
extern void setJetpackHeight(float value);
extern float fFovValue;
extern float fWeatherValue;
extern float fMeleeSpeed;
extern float fCloudSpeed;
extern float fStarPos;
extern void setFov(float value);
extern float getFov();
extern void setMeleeSpeed(float value);
extern float getMeleeSpeed();
extern void setWeather(float value);
extern float getWeather();
extern void setCloudSpeed(float value);
extern float getCloudSpeed();
extern float getStarPos();
extern void setStarPos(float value);
extern std::uint32_t g_ScriptSpellFormId;
extern void renderAdminTab();
enum class CharacterStateType : std::int32_t {
    OnGround = 0, // self explan 1
    Jumping  = 1, // self explan 2
    InAir    = 2, // self explan 3
    Climbing = 3, // enables only vertical movement
    Flying   = 4, // basically noclip
    Swimming = 5, // slightly falling down
    Floating = 6, // i think this for noclip idk
};
extern void setstate(std::int32_t state);
extern void applystate();
extern bool bcharacterstateenabled;
extern std::int32_t g_CharacterStateSelected;
extern bool bcharacterflyenabled;
extern void renderMenu();
extern bool bDisplayoverlay;
extern bool bMainMenuVisible;
extern void TFC(bool enable);
extern bool bFreecamEnabled;
extern bool bfreecamtoggle;
extern bool bfreecamon;
extern float getTFC();
extern void toggleTFC(float value);
extern void freecamLoop();