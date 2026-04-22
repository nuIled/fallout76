#pragma once

#include "imgui/imgui.h"
#include "../../../core/dll_main/globals.h"
#include <cstdint>
#include <vector>
#include <string>
#include <Windows.h>

namespace ESP {
    enum class FormType : std::uint8_t {
        BGSTextureSet        = 19,  // 0x13
        TESSound             = 28,  // 0x1C
        BGSAcousticSpace     = 30,  // 0x1E
        TESObjectACTI        = 38,  // 0x26
        TESObjectARMO        = 42,  // 0x2A
        TESObjectCONT        = 44,  // 0x2C
        TESObjectDOOR        = 45,  // 0x2D
        TESObjectLIGH        = 47,  // 0x2F
        TESObjectMISC        = 48,  // 0x30
        TESObjectSTAT        = 52,  // 0x34
        BGSStaticCollection  = 53,  // 0x35
        BGSMovableStatic     = 54,  // 0x36
        TESFlora             = 57,  // 0x39
        TESFurniture         = 58,  // 0x3A
        TESObjectWEAP        = 59,  // 0x3B
        TESAmmo              = 60,  // 0x3C
        TESNPC               = 61,  // 0x3D
        TESKey               = 64,  // 0x40
        AlchemyItem          = 65,  // 0x41
        BGSIdleMarker        = 67,  // 0x43
        BGSNote              = 68,  // 0x44
        BGSHazard            = 70,  // 0x46
        BGSBendableSpline    = 71,  // 0x47
        TESLevItem           = 76,  // 0x4C
        TESObjectBook        = 0x29,
        CurrencyObject       = 0x31,
        TESUtilityItem       = 0x42,
        TESObjectRefr        = 0x52,
        TESActor             = 0x55,
        PlayerCharacter      = 0xB7,
        Undefined            = 0xFF,
    };

    enum class ActorState : int {
        Unknown,
        Alive,
        Dead,
        Downed,
    };

    enum class EntityCategory : int {
        Invalid,
        Player,
        Npc,
        Container,
        Junk,
        Plan,
        Magazine,
        Bobblehead,
        Flora,
        Weapon,
        Armor,
        Ammo,
        Aid,
        Misc,
        Mod,
        Other,
    };

    struct Vec2 {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct Vec3 {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        Vec3 operator-(const Vec3& o) const {
            return { x - o.x, y - o.y, z - o.z };
        }

        float Length() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        float DistanceTo(const Vec3& o) const {
            return (*this - o).Length();
        }
    };

    #pragma pack(push, 1)
    struct TESItem {
        std::uintptr_t vtable;
        char pad08[0x10];
        char recordFlagA;
        char pad19[0x07];
        std::uint32_t formId;
        char pad24[0x02];
        std::uint8_t formType;
        char pad27[0x71];
        std::uintptr_t namePtr0098;
        char padA0[0x10];
        std::uintptr_t namePtr00B0;
        std::uintptr_t keywordData00B8;
        std::uintptr_t keywordData00C0;
        char padC8[0x19];
        char omodFlag;
        char padE2[0x0E];
        std::uintptr_t factionArrayPtr;
        char padF8[0x08];
        int factionArraySize;
        char pad104[0x74];
        std::uintptr_t namePtr0178;
        char pad180[0x30];
        std::uintptr_t keywordData01B0;
        std::uintptr_t keywordData01B8;
        std::uintptr_t keywordData01C0;
        char padC8_2[0x10];
        char planFlag;
        char padD9[0x0F];
        std::uintptr_t componentArrayPtr;
        char padF0[0x08];
        int componentArraySize;

        FormType GetFormType() const { return static_cast<FormType>(formType); }
        bool IsPlan() const { return GetFormType() == FormType::TESObjectBook && (planFlag >> 5 & 1); }
        bool IsJunkItem() const { return GetFormType() == FormType::TESObjectMISC && componentArraySize && !(recordFlagA >> 7 & 1); }
        bool IsMod() const { return GetFormType() == FormType::TESObjectMISC && (recordFlagA >> 7 & 1); }
        bool IsMagazine() const { return GetFormType() == FormType::TESObjectBook && !IsPlan(); }
        bool IsBobblehead() const { 
            return GetFormType() == FormType::TESObjectMISC && 
                   !IsJunkItem() && 
                   !IsMod() && 
                   componentArraySize == 0;
        }
    };

    struct TESObjectRefr {
        std::uintptr_t vtable;             // 0x00
        char pad08[0x08];                  // 0x08
        char harvestFlagA;                 // 0x10
        char pad11[0x08];                  // 0x11
        char harvestFlagB;                 // 0x19
        char pad1A[0x06];                  // 0x1A
        std::uint32_t formId;              // 0x20
        char pad24[0x02];                     // 0x24
        std::uint8_t formType;              // 0x26
        
        FormType GetFormType() const { return static_cast<FormType>(formType); }
        char pad27[0x11];                  // 0x27
        char idValue[4];                    // 0x38
        char pad3C[0x14];                  // 0x3C
        std::uintptr_t vtable0050;          // 0x50
        char pad58[0x08];                  // 0x58
        float pitch;                        // 0x60
        char pad64[0x04];                  // 0x64
        float yaw;                          // 0x68
        char pad6C[0x04];                  // 0x6C
        Vec3 position;                      // 0x70
        char pad7C[0x04];                  // 0x7C
        std::uintptr_t inventoryPtr;        // 0x80
        char pad88[0x08];                  // 0x88
        std::uintptr_t actorCorePtr;        // 0x90
        char pad98[0x10];                  // 0x98
        std::uintptr_t cellPtr;             // 0xA8
        std::uintptr_t skeletonPtr;         // 0xB0
        char padB8[0x08];                  // 0xB8
        std::uintptr_t baseObjectPtr;       // 0xC0
        char padC8[0x0E];                  // 0xC8
        char spawnFlag;                     // 0xD6
        char padD7[0xD1];                  // 0xD7
        char movementFlag;                  // 0x1A8
        char sprintFlag;                    // 0x1A9
        char healthFlag;                    // 0x1AA
    };

    struct ActorSnapshot {
        std::uintptr_t actorCorevtable;
        char actorCorePad[0x98];
        std::uintptr_t vtable;
        char padA8[0x30];
        char isInvulnerable;
        char unk;
        char unk2;
        char isEssential;
        char isProtected;
        char isPlayerProtected;
        char padDE[0x3A];
        float maxHealth;
        float modifiedHealth;
        char pad120[0x04];
        float lostHealth;
        char pad128[0xA0];
        std::uint8_t epicRank;
    };

    struct Camera {
        std::uintptr_t vtable;
        char pad08[0x68];
        Vec3 forward;            // 0x70
        char pad7C[0x94];        // 0x7C to 0x110
        Vec3 origin;             // 0x110
        char pad11C[0x20];       // 0x11C to 0x13C
        float viewMatrix[4][4];  // 0x13C
    };

    struct TESObjectCell {
        std::uintptr_t vtable;
        char pad08[0x18];
        std::uint32_t formId;
        char pad24[0x40];
        std::uint64_t isInterior;
        char pad6C[0x04];        // 0x6C to 0x70
        char loadedState;        // 0x70
        char pad71[0x2F];
        std::uintptr_t objectListBeginPtr;
        std::uintptr_t objectListEndPtr;
    };

    struct LoadedAreaManager {
        char pad00[0x80];
        std::uintptr_t interiorCellBegin;
        std::uintptr_t interiorCellEnd;
        char pad90[0x18];
        std::uintptr_t exteriorCellBegin;
        std::uintptr_t exteriorCellEnd;
    };

    struct FalloutMain {
        std::uintptr_t vtable;
        char pad08[0x380];       // 0x08 to 0x388
        std::uintptr_t platformSessionMgr;  // 0x388
    };

    struct PlatformSessionManager {
        std::uintptr_t vtable;
        char pad08[0x90];        // 0x08 to 0x98
        std::uintptr_t clientAccountMgr;  // 0x98
    };

    struct ClientAccountManager {
        std::uintptr_t vtable;
        char pad08[0x38];
        std::uintptr_t clientAccountArray;
        char pad48[0x08];
        int arraySizeA;
        char pad54[0x04];
        int arraySizeB;
    };

    struct ClientAccountBuffer {
        std::uintptr_t namePtr;
        std::uintptr_t clientAccountPtr;
    };

    struct ClientAccount {
        std::uintptr_t vtable;        // 0x00
        char pad08[0x08];             // 0x08 to 0x010
        BYTE nameData[0x10];          // 0x010 to 0x020
        int nameLength;               // 0x020
        char pad24[0x58];             // 0x024 to 0x07C (0x07C - 0x024 = 0x58)
        std::uint32_t formId;         // 0x07C
    };

    struct WorldSpace {
        char pad00[0xF8];
        std::uintptr_t skyCellPtr;
    };
    #pragma pack(pop)

    struct EntityData {
        std::uintptr_t ptr = 0;
        std::uint32_t formId = 0;
        std::string name;
        Vec3 position;
        float distance = 0.0f;
        int distanceM = 0;
        Vec2 screenPos;
        bool onScreen = false;
        float health = 0.0f;
        float maxHealth = 0.0f;
        bool isPlayer = false;
        bool isNPC = false;
        bool isEnemy = false;
        EntityCategory category = EntityCategory::Invalid;
        ActorState actorState = ActorState::Unknown;
    };

    namespace Config {
        extern bool enable;
        extern bool showPlayers;
        extern bool showNPCs;
        extern bool showEnemies;
        extern bool showContainers;
        extern bool showJunk;
        extern bool showPlans;
        extern bool showMagazines;
        extern bool showBobbleheads;
        extern bool showFlora;
        extern bool showWeapons;
        extern bool showArmor;
        extern bool showAmmo;
        extern bool showAid;
        extern bool showMisc;
        extern bool showMods;
        extern bool showOther;
        extern bool showBoxes;
        extern bool showSnaplines;
        extern bool showEntityInfo; 
        extern bool showHealthBar;
        extern float screenClampMargin;
        extern float maxDistance;
        extern float fov;
        extern float screenOffsetX;
        extern float screenOffsetY;
        extern float boxOffsetX;
        extern float boxOffsetY;
        extern float containerScreenOffsetY;
        extern float boxScaleX;
        extern float boxScaleY;
        extern bool dynamicBoxScaling;
        extern float minBoxScale;
        extern float maxBoxScale;
        extern float scalingDistance;
        extern ImColor playerColor;
        extern ImColor npcColor;
        extern ImColor enemyColor;
        extern ImColor containerColor;
        extern ImColor junkColor;
        extern ImColor planColor;
        extern ImColor magazineColor;
        extern ImColor bobbleheadColor;
        extern ImColor floraColor;
        extern ImColor weaponColor;
        extern ImColor armorColor;
        extern ImColor ammoColor;
        extern ImColor aidColor;
        extern ImColor miscColor;
        extern ImColor modColor;
        extern ImColor otherColor;
        extern ImColor nameColor;
        extern ImColor distanceColor;
        extern ImColor healthColor;
    }

    bool readCam(Camera& out);
    Vec2 W2S(const Vec3& pos, float screenW, float screenH, bool& visible);
    Vec3 getLocPlayerXYZpos();
    void UpdateESP();
    void RenderESP();
    std::vector<EntityData> GetPlayers();
    std::vector<EntityData> GetNPCs();

    struct ESPDebugInfo {
        bool enabled = false;
        bool cameraValid = false;
        Vec3 localPlayerPos = {};
        Vec3 cameraOrigin = {};
        Vec3 cameraForward = {};
        int playerCount = 0;
        int npcCount = 0;
        float screenWidth = 0.0f;
        float screenHeight = 0.0f;
        std::uintptr_t cameraPtr = 0;
        float viewMatrix[4][4] = {};
    };
    ESPDebugInfo GetDebugInfo();
}