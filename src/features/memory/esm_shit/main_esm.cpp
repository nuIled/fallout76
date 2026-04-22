#include "../../../core/dll_main/globals.h"
#include "../../../core/sig_main/sig.h"
#include "../../../gui/menu_main/gui.h"
#include <cstdio>
#include <thread>
#include <chrono>

struct ESMWeaponModDebug {
    uintptr_t esmAddr;
    uintptr_t weaponPoolPtr;
    size_t assetCount;
    int weaponsProcessed;
    int writesApplied;
    int hadException;
    uintptr_t sampleWeap[4];
    char probeResult[256];
};

FORM_ENUM_STRING* g_FormEnumString = nullptr;
bool bWepInfAmmo = false;
bool bWepNoReload = false;
bool bWepNoAP = false;
bool bWepFFREnable = false;
bool bWepNoAttackDelay = false;
float fWepFFR = 0.0f;
bool bWepModsCont = false;

void dumpenums() {
    uintptr_t addr = sigMain("48 8B 89 ? 00 00 00 4C 8D 3D", 0, 0xA, 0);
    if (!addr) return;
    g_FormEnumString = reinterpret_cast<FORM_ENUM_STRING*>(addr);

    FILE* f = nullptr;
    if (fopen_s(&f, "F76FormEnum.c", "w") != 0 || !f) return;
    fprintf(f, "enum FormType\n");
    fprintf(f, "{\n");
    for (FORM_ENUM_STRING* itr = g_FormEnumString; itr->pFormString; itr++) {
        if (itr != g_FormEnumString && (itr - 1)->cFormID != itr->cFormID - 1)
            break;
        fprintf(f, "    kFormType_%s = %i,\n", itr->pFormString, itr->cFormID);
    }
    fprintf(f, "    kFormType_kFormType_Max\n");
    fprintf(f, "};\n");
    fclose(f);
}

static bool IsValidPtr(uintptr_t p) {
    return p >= 0x10000 && p < 0x7FFFFFFFFFFF;
}
/* ai bs from sussy but when shits not wrapped, i go to desktop, so it stays! until i rewrite this peice of shit... */
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

template <typename T>
static bool saferead(uintptr_t addr, T* out) {
    if (!out || addr == 0) return false;
    SIZE_T bytesRead = 0;
    return ReadProcessMemory(GetCurrentProcess(),
                             reinterpret_cast<LPCVOID>(addr),
                             out,
                             sizeof(T),
                             &bytesRead) != 0 &&
           bytesRead == sizeof(T);
}

static constexpr uintptr_t kAmmoPtrOffset      = 0x2C8;
static constexpr uintptr_t kAttackSecondsOffset = 0x330;
static constexpr uintptr_t kReloadOffset       = 0x334;
static constexpr uintptr_t kFireRateOffset     = 0x33C; // 4 8 2000
static constexpr uintptr_t kAttackDelayOffset  = 0x34C + 0x8; // im not too sure if this works?
static constexpr uintptr_t kAPCostOffset       = 0x374;
static constexpr int kFormType_WEAP = 59;

struct PoolEntry {
    uintptr_t assets;
    uintptr_t elementSize;
    uintptr_t count;
};

static bool safewrite(uintptr_t addr, float value) {
    if (!issafewrite(addr)) return false;
    DWORD oldProtect;
    if (!VirtualProtect(reinterpret_cast<void*>(addr), sizeof(float), PAGE_READWRITE, &oldProtect))
        return false;
    *reinterpret_cast<float*>(addr) = value;
    VirtualProtect(reinterpret_cast<void*>(addr), sizeof(float), oldProtect, &oldProtect);
    return true;
}

static uintptr_t GetValidEsmAddr(int formType, ESMWeaponModDebug* dbg) {/* thanks scar ! */
    uintptr_t esmBase = sigMain("48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 05 ? ? ? ? 49 8B ? 48 8B C8", 0, 3, 0);
    if (!esmBase) {
        if (dbg) dbg->esmAddr = 0;
        return 0;
    }

    uintptr_t esmAddr = esmBase + 0x70;
    PoolEntry pool{};
    const uintptr_t poolEntryAddr = esmAddr + static_cast<uintptr_t>(formType) * 0x18;
    if (!saferead(poolEntryAddr, &pool) || !IsValidPtr(pool.assets) || pool.count == 0 || pool.count > 100000) {
        if (dbg) dbg->esmAddr = 0;
        return 0;
    }

    if (dbg) dbg->esmAddr = esmAddr;
    return esmAddr;
}

static void applyesmpatch() {
    uintptr_t esmAddr = GetValidEsmAddr(kFormType_WEAP, nullptr);
    if (!esmAddr) return;
    PoolEntry pool{};
    const uintptr_t poolEntryAddr = esmAddr + static_cast<uintptr_t>(kFormType_WEAP) * 0x18;
    if (!saferead(poolEntryAddr, &pool) || !IsValidPtr(pool.assets) || pool.count == 0 || pool.count > 100000) return;
    for (size_t i = 0; i < pool.count; i++) {//thx
        uintptr_t weap = 0;
        if (!saferead(pool.assets + i * 8, &weap)) continue;
        if (!weap || !IsValidPtr(weap)) continue;

        uintptr_t pAmmoAddr = weap + kAmmoPtrOffset;
        uintptr_t ammoVal = 0; //sigma
        if (!saferead(pAmmoAddr, &ammoVal) || !ammoVal) continue;

        if (issafewrite(pAmmoAddr)) {
            DWORD old;
            if (VirtualProtect(reinterpret_cast<void*>(pAmmoAddr), sizeof(uintptr_t), PAGE_READWRITE, &old)) {
                *reinterpret_cast<uintptr_t*>(pAmmoAddr) = 0;
                VirtualProtect(reinterpret_cast<void*>(pAmmoAddr), sizeof(uintptr_t), old, &old);
            }
        }

        safewrite(weap + kAttackSecondsOffset, 0.0f);
        safewrite(weap + kReloadOffset, 1000.0f);
        float fireRateValue = fWepFFR > 0.0f ? (fWepFFR * 0.5f) : 2.5f;// YOU HAVE BEEN DISCONNECTED DUE TO MODIFIED HOMOSEXUAL FILES!
        safewrite(weap + kFireRateOffset, fireRateValue);
        if (bWepNoAttackDelay) safewrite(weap + kAttackDelayOffset, 0.0f);
        safewrite(weap + kAPCostOffset, 0.0f);
    }
}

void runesmpatcher() {
    while (true) {
        if (bWepModsCont) {
            applyMods();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
}

void applyMods() {
    applyesmpatch();
}