#include "../esp_main/esp.h"
#include "imgui/imgui.h"

namespace ESP {
    namespace Config {
        bool enable = false;
        bool showPlayers = false;
        bool showNPCs = false;
        bool showEnemies = false;
        bool showContainers = false;
        bool showJunk = false;
        bool showPlans = false;
        bool showMagazines = false;
        bool showBobbleheads = false;
        bool showFlora = false;
        bool showWeapons = false;
        bool showArmor = false;
        bool showAmmo = false;
        bool showAid = false;
        bool showMisc = false;
        bool showMods = false;
        bool showOther = false;
        bool showBoxes = false;
        bool showSnaplines = false;
        bool showEntityInfo = false;
        bool showHealthBar = false;
        float screenClampMargin = 0.0f;
        float maxDistance = 100.0f;
        float fov = 90.0f;
        float screenOffsetX = 0.0f;
        float screenOffsetY = 0.0f;
        float boxOffsetX = 0.0f;
        float boxOffsetY = 0.0f;
        float containerScreenOffsetY = 12.0f;
        float boxScaleX = 1.0f;
        float boxScaleY = 1.0f;
        bool dynamicBoxScaling = false;
        float minBoxScale = 0.5f;
        float maxBoxScale = 1.2f;
        float scalingDistance = 1000.0f;
        ImColor playerColor = ImColor(0, 128, 255, 255);
        ImColor npcColor = ImColor(0, 255, 0, 255);
        ImColor enemyColor = ImColor(255, 0, 0, 255);
        ImColor containerColor = ImColor(255, 255, 0, 255);
        ImColor junkColor = ImColor(128, 128, 128, 255);
        ImColor planColor = ImColor(255, 165, 0, 255);
        ImColor magazineColor = ImColor(255, 0, 255, 255);
        ImColor bobbleheadColor = ImColor(255, 192, 203, 255);
        ImColor floraColor = ImColor(0, 255, 128, 255);
        ImColor weaponColor = ImColor(255, 0, 0, 255);
        ImColor armorColor = ImColor(0, 0, 255, 255);
        ImColor ammoColor = ImColor(255, 255, 0, 255);
        ImColor aidColor = ImColor(0, 255, 255, 255);
        ImColor miscColor = ImColor(192, 192, 192, 255);
        ImColor modColor = ImColor(128, 0, 128, 255);
        ImColor otherColor = ImColor(255, 255, 255, 255);
        ImColor nameColor = ImColor(255, 0, 0, 255);
        ImColor distanceColor = ImColor(255, 255, 255, 255);
        ImColor healthColor = ImColor(255, 255, 255, 255);
    }
}