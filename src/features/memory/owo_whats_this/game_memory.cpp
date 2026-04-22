#include "../../../core/dll_main/globals.h"
#include "../../esp/esp_main/esp.h"
#include "../../../gui/menu_main/gui.h"
#include <cstdio>
#include <cstring>
#include <unordered_set>
#include <vector>

static const wchar_t* fallout = L"Fallout76.exe";// imsocool

uintptr_t getbase() {
    static uintptr_t cached = 0;
    if (cached == 0)
        cached = reinterpret_cast<uintptr_t>(GetModuleHandleW(fallout));// much cleaner.
    return cached;
}

static bool IsValidPtr(uintptr_t p) {
    return p >= 0x10000 && p < 0x7FFFFFFFFFFF;
}

static uintptr_t getLocPlayer() {
    uintptr_t ptrAddr = getlocal_player();
    if (!IsValidPtr(ptrAddr)) return 0;
    __try {
        uintptr_t player = *reinterpret_cast<uintptr_t*>(ptrAddr);
        return (player && IsValidPtr(player)) ? player : 0;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

bool bcharacterstateenabled = false; // hahah ctd hahah
std::int32_t g_CharacterStateSelected = 0;// 0 is default iirc
bool bcharacterflyenabled = false;

void setstate(std::int32_t state) {
    __try {

        uintptr_t basePtrAddr = gethavok_physics();
        if (!basePtrAddr || !IsValidPtr(basePtrAddr)) return;
        uintptr_t charController = *reinterpret_cast<uintptr_t*>(basePtrAddr);
        if (!charController || !IsValidPtr(charController)) return;

        *reinterpret_cast<std::int32_t*>(charController + 0x300) = state;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}

void applystate() {
    bool lastFlyEnabled = false;
    bool lastStateEnabled = false;
    while (true) {
        if (bNoclipEnabled) {
            setstate(static_cast<std::int32_t>(CharacterStateType::Floating)); // this is state 6 btw, you wanna use this for no rubberbanding
            lastFlyEnabled = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        else if (bcharacterflyenabled) {
            setstate(static_cast<std::int32_t>(CharacterStateType::Floating));
            lastFlyEnabled = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        else {
            if (lastFlyEnabled) {
                setstate(static_cast<std::int32_t>(CharacterStateType::OnGround));
                lastFlyEnabled = false;
            }
            else if (bcharacterstateenabled) {
                setstate(g_CharacterStateSelected);
                lastStateEnabled = true;
                std::this_thread::sleep_for(std::chrono::milliseconds(25));

            }
        }
    }
}

void velstate() {
    bVelEnabled = bVelFrozen || bVelNoclipFrozen;
}

void applyVel() {
    while (!bVelDisabled) {
        if (bVelEnabled) {
            float appliedValue = bVelNoclipFrozen ? 0.422f : fVelValue;
            getVel(appliedValue);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

void getVel(float value) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;

    __try {
        uintptr_t firstLevelAddress = gethavok_physics();
        uintptr_t firstLevelValue = *reinterpret_cast<uintptr_t*>(firstLevelAddress);
        if (!firstLevelValue) return;

        uintptr_t secondLevelValue = *reinterpret_cast<uintptr_t*>(firstLevelValue + 0x4C0);
        if (!secondLevelValue) return;

        *reinterpret_cast<float*>(secondLevelValue + 0xA8) = value;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

float getVelocity() {
    uintptr_t baseModule = getbase();
    if (!baseModule) return 0.0f;

    __try {
        uintptr_t firstLevelAddress = gethavok_physics();
        uintptr_t firstLevelValue = *reinterpret_cast<uintptr_t*>(firstLevelAddress);
        if (!firstLevelValue) return 0.0f;

        uintptr_t secondLevelValue = *reinterpret_cast<uintptr_t*>(firstLevelValue + 0x4C0);
        if (!secondLevelValue) return 0.0f;

        return *reinterpret_cast<float*>(secondLevelValue + 0xA8);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0.0f;
    }
}

void cacheVA() {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;

    __try {
        uintptr_t base = getlocal_player();
        if (base < 0x10000 || base >= 0x7FFFFFFFFFFF) return;

        uintptr_t ptr = *reinterpret_cast<uintptr_t*>(base);

        if (ptr && ptr >= 0x10000 && ptr < 0x7FFFFFFFFFFF) {
            uiViewAngleOffset = ptr;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

void freecamLoop() {
    static bool lastKeyState = false;
    static bool lastAppliedState = false;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (!bFreecamEnabled) {
            if (lastAppliedState) {
                TFC(false);
                lastAppliedState = false;
            }
            bfreecamon = false;
            lastKeyState = false;
            continue;
        }
        bool currentKeyState = (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0;
        bool keyPressed = currentKeyState && !lastKeyState;
        lastKeyState = currentKeyState;
        if (bfreecamtoggle) {
            if (keyPressed) {
                bfreecamon = !bfreecamon;
            }
        } else {
            bfreecamon = currentKeyState;
        }

        if (bfreecamon != lastAppliedState) {
            TFC(bfreecamon);
            lastAppliedState = bfreecamon;
        }
    }
}

void noclipLoop() {
    static bool lastKeyState = false;
    static bool wasNoclipActive = false;

    while (true) {
        bool currentKeyState = (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) != 0;
        bool keyPressed = currentKeyState && !lastKeyState;
        lastKeyState = currentKeyState;
        bool noclipActive = false;

        if (bnoclipenabled) {
            if (bnocliptoggle) {
                if (keyPressed) {
                    bnoclipon = !bnoclipon;
                }
                noclipActive = bnoclipon;
            }
            else {
                noclipActive = currentKeyState;
            }
        }

        bNoclipEnabled = noclipActive;
        wasNoclipActive = noclipActive;

        if (noclipActive) {
            if (!bVelNoclipFrozen) {
                bVelNoclipFrozen = true;
                velstate();
            }
            if (!wasNoclipActive) {
                // maybe use float handler..?  at the time of typing this, i have not yet tested
                setstate(static_cast<std::int32_t>(CharacterStateType::Flying));
            }
        }
        else if (bVelNoclipFrozen) {
            bVelNoclipFrozen = false;
            velstate();
            if (wasNoclipActive) {
                setstate(static_cast<std::int32_t>(CharacterStateType::OnGround));
            }
        }

        if (noclipActive) {
            uintptr_t baseModule = getbase();
            if (!baseModule) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            static bool viewAnglesInitialized = false;
            if (!viewAnglesInitialized) {
                cacheVA();
                viewAnglesInitialized = true;
            }

            __try {
                uintptr_t firstLevelAddress = gethavok_physics();
                uintptr_t firstLevelValue = *reinterpret_cast<uintptr_t*>(firstLevelAddress);
                if (!firstLevelValue) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }

                uintptr_t secondLevelPointer = *reinterpret_cast<uintptr_t*>(firstLevelValue + 0x4C0);
                if (!secondLevelPointer) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }

                if (secondLevelPointer < 0x10000 || secondLevelPointer >= 0x7FFFFFFFFFFF) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }

                float currentX = *reinterpret_cast<float*>(secondLevelPointer + 0x74);
                float currentY = *reinterpret_cast<float*>(secondLevelPointer + 0x78);
                float currentZ = *reinterpret_cast<float*>(secondLevelPointer + 0x70);

                float pitch = fCurViewPitch;
                float yaw = fCurViewYaw;
                uintptr_t viewPtr = uiViewAngleOffset;

                if (!viewPtr) {
                    uintptr_t baseAddress = getlocal_player();
                    if (baseAddress >= 0x10000 && baseAddress < 0x7FFFFFFFFFFF) {
                        viewPtr = *reinterpret_cast<uintptr_t*>(baseAddress);
                    }
                }

                if (viewPtr && viewPtr >= 0x10000 && viewPtr < 0x7FFFFFFFFFFF) {
                    pitch = *reinterpret_cast<float*>(viewPtr + 0x60);
                    yaw = *reinterpret_cast<float*>(viewPtr + 0x68);
                }

                float player_speed = fNoclipSpeed * fNoclipSpeedMult;
                float newX = currentX;
                float newY = currentY;
                float newZ = currentZ;

                if (GetAsyncKeyState('W') & 0x8000) {
                    float directionX = cosf(yaw);
                    float directionZ = sinf(yaw);
                    float directionY = -sinf(pitch) * 2.0f;
                    newX += directionX * player_speed;
                    newZ += directionZ * player_speed;
                    newY += directionY * player_speed;
                }

                if (GetAsyncKeyState('S') & 0x8000) {
                    float directionX = -cosf(yaw);
                    float directionZ = -sinf(yaw);
                    float directionY = sinf(pitch) * 2.0f;
                    newX += directionX * player_speed;
                    newZ += directionZ * player_speed;
                    newY += directionY * player_speed;
                }

                if (GetAsyncKeyState('A') & 0x8000) {
                    float directionX = cosf(yaw - 1.570796f);
                    float directionZ = sinf(yaw - 1.570796f);
                    float magnitude = sqrtf(directionX * directionX + directionZ * directionZ);
                    if (magnitude > 0.0f) {
                        directionX /= magnitude;
                        directionZ /= magnitude;
                    }
                    newX += directionX * player_speed;
                    newZ += directionZ * player_speed;
                }

                if (GetAsyncKeyState('D') & 0x8000) {
                    float directionX = cosf(yaw + 1.570796f);
                    float directionZ = sinf(yaw + 1.570796f);
                    float magnitude = sqrtf(directionX * directionX + directionZ * directionZ);
                    if (magnitude > 0.0f) {
                        directionX /= magnitude;
                        directionZ /= magnitude;
                    }
                    newX += directionX * player_speed;
                    newZ += directionZ * player_speed;
                }

                if (secondLevelPointer >= 0x10000 && secondLevelPointer < 0x7FFFFFFFFFFF) {
                    *reinterpret_cast<float*>(secondLevelPointer + 0x74) = newX;
                    *reinterpret_cast<float*>(secondLevelPointer + 0x78) = newY;
                    *reinterpret_cast<float*>(secondLevelPointer + 0x70) = newZ;
                }
            }
            __except (EXCEPTION_EXECUTE_HANDLER) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void setcrits() {
    while (!bCritsDisabled) {
        if (bCritsEnabled) {
            getcrits(100.0f);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    getcrits(0.0f);
}

void getcrits(float value) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;

    __try {
        uintptr_t firstLevelAddress = getlocal_player();
        uintptr_t firstLevelValue = *reinterpret_cast<uintptr_t*>(firstLevelAddress);
        if (!firstLevelValue) return;

        *reinterpret_cast<float*>(firstLevelValue + 0xA90) = value;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

static void setCarryWeight(float value) {
    uintptr_t globalAddr = getlocal_player();
    if (!globalAddr) return;

    __try {
        uintptr_t actorPtr = *reinterpret_cast<uintptr_t*>(globalAddr);
        if (!actorPtr) return;

        uintptr_t componentPtr = *reinterpret_cast<uintptr_t*>(actorPtr + 0x80);
        if (!componentPtr) return;

        *reinterpret_cast<float*>(componentPtr + 0xC8) = value; // liability (might shift under certain conditions idk, i havent bothered investigating)
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

bool bSprintFatEnabled = false;
bool bSprintFatDisabled = false;
float g_CarryWeightValue = 67.0f;
float fSuperJumpVal = 70.0f;// i believe default is 70, lets hope! - second note, prolly cause issues w marsupial... 
bool bSuperJumpEnabled = false;

void setSuperJump(float value) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;

    __try {
        uintptr_t addr = baseModule + superjump;
        *reinterpret_cast<float*>(addr) = value;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

float bJetpackHeightValue = 750.0f;// default

void setJetpackHeight(float value) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;

    __try {
        uintptr_t addr = baseModule + jetpack_height;
        *reinterpret_cast<float*>(addr) = value;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

std::uint32_t g_ScriptSpellFormId = 0;

float fWeatherValue = 11.0f;

float getWeather() {
    uintptr_t baseModule = getbase();
    if (!baseModule) return fWeatherValue;

    __try {
        uintptr_t addr = baseModule + weather;
        fWeatherValue = *reinterpret_cast<float*>(addr);
        return fWeatherValue;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return fWeatherValue;
    }
}

void setWeather(float value) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;

    __try {
        uintptr_t addr = baseModule + weather;
        *reinterpret_cast<float*>(addr) = value;
        fWeatherValue = value;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

float fCloudSpeed = 0.5f;

float getCloudSpeed() {
    uintptr_t baseModule = getbase();
    if (!baseModule) return fCloudSpeed;
    {
        uintptr_t addr = baseModule + cloud;
        fCloudSpeed = *reinterpret_cast<float*>(addr);
        return fCloudSpeed;
    }
}

void setCloudSpeed(float value) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;
    {
        uintptr_t addr = baseModule + cloud;
        *reinterpret_cast<float*>(addr) = value;
        fCloudSpeed = value;
    }
}

float fStarPos = 1.0f;

float getStarPos() {
    uintptr_t baseModule = getbase();
    if (!baseModule) return fStarPos;
    {
        uintptr_t addr = baseModule + stars;
        fStarPos = *reinterpret_cast<float*>(addr);
        return fStarPos;
    }
}

void setStarPos(float value) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;
    {
        uintptr_t addr = baseModule + stars;
        *reinterpret_cast<float*>(addr) = value;
        fStarPos = value;
    }
}


float fMeleeSpeed = 1.0f;

float getMeleeSpeed() {
    uintptr_t baseModule = getbase();
    if (!baseModule) return fMeleeSpeed;

    __try {
        uintptr_t addr = baseModule + melee;
        fMeleeSpeed = *reinterpret_cast<float*>(addr);
        return fMeleeSpeed;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return fMeleeSpeed;
    }
}

void setMeleeSpeed(float value) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;

    __try {
        uintptr_t addr = baseModule + melee;
        *reinterpret_cast<float*>(addr) = value;
        fMeleeSpeed = value;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

float fFovValue = 70.0f;

float getFov() {
    uintptr_t baseModule = getbase();
    if (!baseModule) return fFovValue;

    __try {
        uintptr_t addr = baseModule + fov;
        fFovValue = *reinterpret_cast<float*>(addr);
        return fFovValue;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return fFovValue;
    }
}

void setFov(float value) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;

    __try {
        uintptr_t addr = baseModule + fov;
        *reinterpret_cast<float*>(addr) = value;
        fFovValue = value;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

bool bFreecamEnabled = false;

void TFC(bool enable) { 
    uintptr_t tfcFunc = get_tfc();
    uintptr_t playerCameraGlobal = get_player_camera(); // ripped from erectus
    if (!tfcFunc || !playerCameraGlobal) return;
    __try {
        uintptr_t playerCameraPtr = *reinterpret_cast<uintptr_t*>(playerCameraGlobal);
        if (!playerCameraPtr) return;

        using TfcFn = void(__fastcall*)(uintptr_t, uint8_t);
        auto fn = reinterpret_cast<TfcFn>(tfcFunc);
        fn(playerCameraPtr, enable ? 1u : 0u);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

float getTFC() {
    uintptr_t addr = get_tfc_speed();
    if (!addr) return 0.0f;
    __try {
        return *reinterpret_cast<float*>(addr);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0.0f;
    }
}

void toggleTFC(float value) {
    uintptr_t addr = get_tfc_speed();
    if (!addr) return;
    __try {
        *reinterpret_cast<float*>(addr) = value;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

void runSprintOverweightLoop() {
    while (!bSprintFatDisabled) {
        if (bSprintFatEnabled) {
            float v = g_CarryWeightValue;
            if (v < 0.1f) v = 0.1f;
            if (v > 999.0f) v = 999.0f;
            setCarryWeight(v);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

void getPlayerSize(float& muscular, float& large, float& thin) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;

    muscular = 0.0f;
    large = 0.0f;
    thin = 0.0f;

    __try {
        uintptr_t firstLevelAddress = getplayer_size();
        uintptr_t firstLevelValue = *reinterpret_cast<uintptr_t*>(firstLevelAddress);
        if (!firstLevelValue) return;

        uintptr_t secondLevelValue = *reinterpret_cast<uintptr_t*>(firstLevelValue + 0x68);
        if (!secondLevelValue) return;

        muscular = *reinterpret_cast<float*>(secondLevelValue + 0x2C);
        large = *reinterpret_cast<float*>(secondLevelValue + 0x30);
        thin = *reinterpret_cast<float*>(secondLevelValue + 0x28);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

void setPlayerSize(float muscular, float large, float thin) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;

    __try {
        uintptr_t firstLevelAddress = getplayer_size();
        uintptr_t firstLevelValue = *reinterpret_cast<uintptr_t*>(firstLevelAddress);
        if (!firstLevelValue) return;

        uintptr_t secondLevelValue = *reinterpret_cast<uintptr_t*>(firstLevelValue + 0x68);
        if (!secondLevelValue) return;

        *reinterpret_cast<float*>(secondLevelValue + 0x2C) = muscular;
        *reinterpret_cast<float*>(secondLevelValue + 0x30) = large;
        *reinterpret_cast<float*>(secondLevelValue + 0x28) = thin;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

bool getWorldCoords(float& x, float& y, float& z) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return false;

    __try {
        uintptr_t baseAddress = getlocal_player();
        if (baseAddress < 0x10000 || baseAddress >= 0x7FFFFFFFFFFF) return false;

        uintptr_t firstLevel = *reinterpret_cast<uintptr_t*>(baseAddress);
        if (!firstLevel || firstLevel < 0x10000 || firstLevel >= 0x7FFFFFFFFFFF) return false;

        x = *reinterpret_cast<float*>(firstLevel + 0xAD8);
        y = *reinterpret_cast<float*>(firstLevel + 0xADC);
        z = *reinterpret_cast<float*>(firstLevel + 0xAE0);

        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool getHavokPosition(float& x, float& y, float& z) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return false;

    __try {
        uintptr_t firstLevelAddress = gethavok_physics();
        if (firstLevelAddress < 0x10000 || firstLevelAddress >= 0x7FFFFFFFFFFF) return false;

        uintptr_t firstLevelValue = *reinterpret_cast<uintptr_t*>(firstLevelAddress);
        if (!firstLevelValue || firstLevelValue < 0x10000 || firstLevelValue >= 0x7FFFFFFFFFFF) return false;

        uintptr_t secondLevelValue = *reinterpret_cast<uintptr_t*>(firstLevelValue + 0x4C0);
        if (!secondLevelValue || secondLevelValue < 0x10000 || secondLevelValue >= 0x7FFFFFFFFFFF) return false;

        x = *reinterpret_cast<float*>(secondLevelValue + 0x74);
        y = *reinterpret_cast<float*>(secondLevelValue + 0x78);
        z = *reinterpret_cast<float*>(secondLevelValue + 0x70);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool writeHavokPosition(float x, float y, float z) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return false;

    __try {
        uintptr_t firstLevelAddress = gethavok_physics();
        if (firstLevelAddress < 0x10000 || firstLevelAddress >= 0x7FFFFFFFFFFF) return false;

        uintptr_t firstLevelValue = *reinterpret_cast<uintptr_t*>(firstLevelAddress);
        if (!firstLevelValue || firstLevelValue < 0x10000 || firstLevelValue >= 0x7FFFFFFFFFFF) return false;

        uintptr_t secondLevelValue = *reinterpret_cast<uintptr_t*>(firstLevelValue + 0x4C0);
        if (!secondLevelValue || secondLevelValue < 0x10000 || secondLevelValue >= 0x7FFFFFFFFFFF) return false;

        *reinterpret_cast<float*>(secondLevelValue + 0x74) = x;
        *reinterpret_cast<float*>(secondLevelValue + 0x78) = y;
        *reinterpret_cast<float*>(secondLevelValue + 0x70) = z;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool getViewModel(float& pitch, float& yaw) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return false;

    __try {
        uintptr_t baseAddress = getlocal_player();
        if (baseAddress < 0x10000 || baseAddress >= 0x7FFFFFFFFFFF) return false;

        uintptr_t viewPtr = *reinterpret_cast<uintptr_t*>(baseAddress);
        if (!viewPtr || viewPtr < 0x10000 || viewPtr >= 0x7FFFFFFFFFFF) return false;

        pitch = *reinterpret_cast<float*>(viewPtr + 0x60);
        yaw = *reinterpret_cast<float*>(viewPtr + 0x68);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool setViewModel(float pitch, float yaw) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return false;

    __try {
        uintptr_t baseAddress = getlocal_player();
        if (baseAddress < 0x10000 || baseAddress >= 0x7FFFFFFFFFFF) return false;

        uintptr_t viewPtr = *reinterpret_cast<uintptr_t*>(baseAddress);
        if (!viewPtr || viewPtr < 0x10000 || viewPtr >= 0x7FFFFFFFFFFF) return false;

        *reinterpret_cast<float*>(viewPtr + 0x60) = pitch;
        *reinterpret_cast<float*>(viewPtr + 0x68) = yaw;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool playerSpeedboostStart = false;
bool playerSpeedboostStop = false;

void getSpeed(float value) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;

    __try {
        uintptr_t baseAddress = getplayer_speed();
        uintptr_t firstLevel = *reinterpret_cast<uintptr_t*>(baseAddress);
        if (!firstLevel) return;
        uintptr_t secondLevel = *reinterpret_cast<uintptr_t*>(firstLevel + 0xA0);
        if (!secondLevel) return;
        *reinterpret_cast<float*>(secondLevel + 0x60) = value;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

void setPlayerSpeed() {
    constexpr float playerSpeedApplied = 150.0f;
    // i've removed player disabled speed, it would eventually return the default value after the server check applies :D
    while (!playerSpeedboostStop) {
        if (playerSpeedboostStart) {
            getSpeed(playerSpeedApplied);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

static bool issafewrite(uintptr_t addr) {
    MEMORY_BASIC_INFORMATION mbi{};
    if (VirtualQuery(reinterpret_cast<void*>(addr), &mbi, sizeof(mbi)) == 0)
        return false;
    if (mbi.State != MEM_COMMIT)
        return false;
    if (mbi.Protect == PAGE_NOACCESS || mbi.Protect == PAGE_EXECUTE)
        return false;
    return true;
}
bool bAmmoEnabled = false;
bool bAmmoActive = false;
bool bAmmoStop = false;
bool bMeleeAutoEnabled = false;

void setAmmoValue(uint32_t value) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;
    __try { // im pretty sure this is better, it seems much more reliable now, before it wasnt aalways writing
        uintptr_t baseAddress = baseModule + omod;
        uintptr_t firstLevelValue = *reinterpret_cast<uintptr_t*>(baseAddress);
        if (!firstLevelValue) return;

        uintptr_t secondLevelAddress = firstLevelValue + 0x1C0;
        uintptr_t secondLevelValue = *reinterpret_cast<uintptr_t*>(secondLevelAddress);
        if (!secondLevelValue) return;

        uintptr_t thirdLevelAddress = secondLevelValue + 0x40;
        uintptr_t thirdLevelValue = *reinterpret_cast<uintptr_t*>(thirdLevelAddress);
        if (!thirdLevelValue) return;

        *reinterpret_cast<uint32_t*>(thirdLevelValue + 0x20) = value;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

void setmeleeautoval(uint32_t value) {
    uintptr_t baseModule = getbase();
    if (!baseModule) return;

    __try {
        uintptr_t baseAddress = baseModule + omod;
        uintptr_t firstLevelValue = *reinterpret_cast<uintptr_t*>(baseAddress);
        if (!firstLevelValue) return;

        uintptr_t secondLevelAddress = firstLevelValue + 0x1C0;
        uintptr_t secondLevelValue = *reinterpret_cast<uintptr_t*>(secondLevelAddress);
        if (!secondLevelValue) return;

        uintptr_t thirdLevelAddress = secondLevelValue + 0x40;
        uintptr_t thirdLevelValue = *reinterpret_cast<uintptr_t*>(thirdLevelAddress);
        if (!thirdLevelValue) return;

        *reinterpret_cast<uint32_t*>(thirdLevelValue + 0x320) = value;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

void runomodpatcher() {// hella makeshift hahah
    while (!bAmmoStop) {
        int sleepMs = 50;
        if (bcustomOmodEnabled && bCustomOmodValApply) {
            setAmmoValue(uiCustomOmodVal);
            sleepMs = 25;
        }
        else if (bAmmoActive) {
            setAmmoValue(0x00113689);
            sleepMs = 25;
        }
        else {
            setAmmoValue(0x0037D0C1);
            sleepMs = 50;
        }

        if (bMeleeAutoEnabled)
            setmeleeautoval(0x001F660D);
        else
            setmeleeautoval(0x005117BF);

        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
    }
    setAmmoValue(0x0037D0C1);
    setmeleeautoval(0x005117BF);
}