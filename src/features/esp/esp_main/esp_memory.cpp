#include "esp_internal.h"
#include "../../../core/dll_main/globals.h"
#include <memory>
#include <cstring>

namespace ESP {
    static std::uint32_t Crc32(std::uint32_t formId) noexcept {
        static constexpr std::uint32_t table[256] = {
            0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,
            0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91,
            0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7,
            0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,
            0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,
            0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,
            0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,
            0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D,
            0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433,
            0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01,
            0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,
            0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,
            0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,
            0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
            0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F,
            0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD,
            0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683,
            0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,
            0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,
            0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,
            0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,
            0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79,
            0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F,
            0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,
            0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,
            0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,
            0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,
            0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
            0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB,
            0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9,
            0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF,
            0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D,
        };
        const auto* bytes = reinterpret_cast<const std::uint8_t*>(&formId);
        std::uint32_t hash = 0;
        for (int i = 0; i < 4; i++)
            hash = (hash >> 8) ^ table[(hash ^ bytes[i]) & 0xFF];
        return hash;
    }

    std::string readEntName(std::uintptr_t namePtr) {
        std::string result;
        if (!IsValidPtr(namePtr)) return result;
        std::size_t nameLength = 0;
        auto ptr = namePtr;
        if (!Rpm(ptr + 0x10, &nameLength, sizeof nameLength)) return result;
        if (nameLength <= 0 || nameLength > 0x7FFF) {
            std::uintptr_t buffer;
            if (!Rpm(ptr + 0x10, &buffer, sizeof buffer)) return result;
            if (!IsValidPtr(buffer)) return result;
            if (!Rpm(buffer + 0x10, &nameLength, sizeof nameLength)) return result;
            if (nameLength <= 0 || nameLength > 0x7FFF) return result;
            ptr = buffer;
        }
        auto nameSize = nameLength + 1;
        auto buf = std::make_unique<char[]>(nameSize);
        if (!Rpm(ptr + 0x18, buf.get(), nameSize)) return result;
        result = buf.get();
        return result;
    }

    static std::string ReadFirstNonEmptyName(std::uintptr_t a, std::uintptr_t b, std::uintptr_t c) {
        if (IsValidPtr(a)) {
            std::string s = readEntName(a);
            if (!s.empty()) return s;
        }
        if (IsValidPtr(b)) {
            std::string s = readEntName(b);
            if (!s.empty()) return s;
        }
        if (IsValidPtr(c)) {
            std::string s = readEntName(c);
            if (!s.empty()) return s;
        }
        return {};
    }

    std::string getBaseObjName(const TESItem& item) {
        std::uintptr_t namePtr = 0;
        switch (item.GetFormType()) {
        case FormType::AlchemyItem:
        case FormType::TESObjectARMO:
        case FormType::TESObjectBook:
        case FormType::TESObjectMISC:
        case FormType::CurrencyObject:
        case FormType::TESFlora:
        case FormType::TESObjectWEAP:
        case FormType::TESAmmo:
        case FormType::TESUtilityItem:
        case FormType::BGSNote:
        case FormType::TESKey:
            namePtr = item.namePtr0098;
            break;
        case FormType::TESObjectCONT:
            namePtr = item.namePtr00B0;
            break;
        case FormType::TESNPC:
        case FormType::PlayerCharacter:
            return ReadFirstNonEmptyName(item.namePtr0178, item.namePtr00B0, item.namePtr0098);
        default:
            return ReadFirstNonEmptyName(item.namePtr0178, item.namePtr00B0, item.namePtr0098);
        }
        return readEntName(namePtr);
    }

    EntityCategory Classify(const TESItem& base) {
        switch (base.GetFormType()) {
        case FormType::BGSIdleMarker:
        case FormType::BGSStaticCollection:
        case FormType::TESObjectLIGH:
        case FormType::TESObjectSTAT:
        case FormType::BGSMovableStatic:
        case FormType::TESSound:
        case FormType::BGSTextureSet:
        case FormType::BGSBendableSpline:
        case FormType::BGSAcousticSpace:
        case FormType::TESLevItem:
        case FormType::TESObjectACTI:
        case FormType::TESObjectDOOR:
        case FormType::BGSHazard:
            return EntityCategory::Invalid;
        case FormType::TESNPC: return EntityCategory::Npc;
        case FormType::TESObjectCONT: return EntityCategory::Container;
        case FormType::TESObjectMISC:
            if (base.IsBobblehead()) return EntityCategory::Bobblehead;
            if (base.IsJunkItem()) return EntityCategory::Junk;
            if (base.IsMod()) return EntityCategory::Mod;
            return EntityCategory::Misc;
        case FormType::TESObjectBook:
            if (base.IsPlan()) return EntityCategory::Plan;
            if (base.IsMagazine()) return EntityCategory::Magazine;
            return EntityCategory::Misc;
        case FormType::TESFlora: return EntityCategory::Flora;
        case FormType::TESObjectWEAP: return EntityCategory::Weapon;
        case FormType::TESObjectARMO: return EntityCategory::Armor;
        case FormType::TESAmmo: return EntityCategory::Ammo;
        case FormType::AlchemyItem:
        case FormType::TESUtilityItem: return EntityCategory::Aid;
        case FormType::TESKey:
        case FormType::BGSNote:
        case FormType::TESFurniture: return EntityCategory::Misc;
        case FormType::CurrencyObject: return EntityCategory::Misc;
        case FormType::TESObjectRefr:
        case FormType::TESActor:
        case FormType::PlayerCharacter: return EntityCategory::Invalid;
        default: return EntityCategory::Other;
        }
    }

    ActorState getAS(const TESObjectRefr& refr) {
        auto ft = refr.GetFormType();
        if (ft != FormType::PlayerCharacter && ft != FormType::TESActor)
            return ActorState::Unknown;
        auto flag = refr.healthFlag;
        flag &= ~(1 << 7);
        flag &= ~(1 << 6);
        flag &= ~(1 << 5);
        switch (flag) {
        case 0x00: return ActorState::Alive;
        case 0x02:
        case 0x04: return ActorState::Dead;
        case 0x10:
        case 0x12: return ActorState::Downed;
        default: return ActorState::Unknown;
        }
    }

    void readAS(const TESObjectRefr& refr, EntityData& out) {
        if (!IsValidPtr(refr.actorCorePtr)) return;
        std::uintptr_t bufA = 0;
        if (!Rpm(refr.actorCorePtr + 0x70, &bufA, sizeof bufA) || !IsValidPtr(bufA)) return;
        std::uintptr_t bufB = 0;
        if (!Rpm(bufA + 0x8, &bufB, sizeof bufB) || !IsValidPtr(bufB)) return;
        ActorSnapshot snap{};
        if (!Rpm(bufB, &snap, sizeof snap)) return;
        out.maxHealth = snap.maxHealth;
        out.health = snap.maxHealth + snap.modifiedHealth + snap.lostHealth;
    }

    TESObjectRefr readLocPlayer(std::uintptr_t& outPtr) {
        TESObjectRefr result{};
        outPtr = 0;
        std::uintptr_t playerPtr = 0;
        if (!Rpm(getlocal_player(), &playerPtr, sizeof playerPtr)) return result;
        if (!IsValidPtr(playerPtr)) return result;
        Rpm(playerPtr, &result, sizeof result);
        outPtr = playerPtr;
        return result;
    }

    LoadedAreaManager readLAM() {
        LoadedAreaManager result{};
        std::uintptr_t lamAddr = get_loaded_area_manager();
        if (!lamAddr) return result;
        std::uintptr_t ptr = 0;
        if (!Rpm(lamAddr, &ptr, sizeof ptr)) return result;
        if (!IsValidPtr(ptr)) return result;
        Rpm(ptr, &result, sizeof result);
        return result;
    }
    // cancer
    std::vector<TESObjectCell> getLoadedCells() {
        std::vector<TESObjectCell> cells;
        auto mgr = readLAM();
        std::uintptr_t arrPtr = 0;
        std::size_t arrSize = 0;
        if (IsValidPtr(mgr.interiorCellBegin) && IsValidPtr(mgr.interiorCellEnd) && mgr.interiorCellBegin != mgr.interiorCellEnd) {
            arrPtr = mgr.interiorCellBegin;
            arrSize = 2;
        } else if (IsValidPtr(mgr.exteriorCellBegin) && IsValidPtr(mgr.exteriorCellEnd) && mgr.exteriorCellBegin != mgr.exteriorCellEnd) {
            arrPtr = mgr.exteriorCellBegin;
            arrSize = 50;
        } else return cells;
        auto cellPtrs = std::make_unique<std::uintptr_t[]>(arrSize);
        if (!Rpm(arrPtr, cellPtrs.get(), arrSize * sizeof(std::uintptr_t))) return cells;
        for (std::size_t i = 0; i < arrSize; i += 2) {
            TESObjectCell cell{};
            if (Rpm(cellPtrs[i], &cell, sizeof cell)) cells.push_back(cell);
        }
        std::uintptr_t localPlayerPtr = 0;
        if (Rpm(getlocal_player(), &localPlayerPtr, sizeof localPlayerPtr) && IsValidPtr(localPlayerPtr)) {
            std::uintptr_t intermediatePtr = 0;
            if (Rpm(localPlayerPtr + 0xA8, &intermediatePtr, sizeof intermediatePtr) && IsValidPtr(intermediatePtr)) {
                std::uintptr_t worldspacePtr = 0;
                if (Rpm(intermediatePtr + 0x100, &worldspacePtr, sizeof worldspacePtr) && IsValidPtr(worldspacePtr)) {
                    WorldSpace ws{};
                    if (Rpm(worldspacePtr, &ws, sizeof ws) && IsValidPtr(ws.skyCellPtr)) {
                        TESObjectCell skyCell{};
                        if (Rpm(ws.skyCellPtr, &skyCell, sizeof skyCell)) cells.push_back(skyCell);
                    }
                }
            }
        }
        return cells;
    }
    // cancer 2
    std::vector<std::pair<std::uintptr_t, TESObjectRefr>> getCellObj(const TESObjectCell& cell) {
        std::vector<std::pair<std::uintptr_t, TESObjectRefr>> result;
        if (cell.loadedState != 7) return result;
        if (!IsValidPtr(cell.objectListBeginPtr) || !IsValidPtr(cell.objectListEndPtr)) return result;
        const auto count = (cell.objectListEndPtr - cell.objectListBeginPtr) / sizeof(std::uintptr_t);
        if (count == 0 || count > 100000) return result;
        auto ptrs = std::make_unique<std::uintptr_t[]>(count);
        if (!Rpm(cell.objectListBeginPtr, ptrs.get(), count * sizeof(std::uintptr_t))) return result;
        result.reserve(count);
        for (std::size_t i = 0; i < count; i++) {
            if (!IsValidPtr(ptrs[i])) continue;
            TESObjectRefr refr{};
            if (!Rpm(ptrs[i], &refr, sizeof refr)) continue;
            if (!IsValidPtr(refr.baseObjectPtr)) continue;
            result.emplace_back(ptrs[i], refr);
        }
        return result;
    }
    // cancer 3
    std::uintptr_t GetPtrFromFormId(std::uint32_t formId) {
        auto base = GetModuleBase();
        if (!base) return 0;
        std::uintptr_t v1 = 0;
        static constexpr uintptr_t kGetPtrA1 = 0x05E2B438;
        if (!Rpm(base + kGetPtrA1, &v1, sizeof v1) || !IsValidPtr(v1)) return 0;
        std::uint32_t capacity = 0;
        if (!Rpm(v1 + 32, &capacity, sizeof capacity) || !capacity) return 0;
        std::uintptr_t entries = 0;
        if (!Rpm(v1 + 24, &entries, sizeof entries) || !IsValidPtr(entries)) return 0;
        auto index = Crc32(formId) & (capacity - 1);
        std::uint32_t next = 0;
        if (!Rpm(entries + index * 24 + 16, &next, sizeof next)) return 0;
        if (next == 0xFFFFFFFF) return 0;
        auto v9 = capacity;
        for (int i = 0; i < 100; i++) {
            std::uint32_t v10 = 0;
            if (!Rpm(entries + index * 24, &v10, sizeof v10)) return 0;
            if (v10 == formId) { v9 = index; if (v9 != capacity) break; }
            else {
                if (!Rpm(entries + index * 24 + 16, &index, sizeof index)) return 0;
                if (index == capacity) break;
            }
        }
        if (v9 == capacity) return 0;
        std::uintptr_t ptr = 0;
        Rpm(entries + v9 * 24 + 8, &ptr, sizeof ptr);
        return ptr;
    }
    // cancer 5 and a half
    std::string readPlayerName(const ClientAccount& account) {
        std::string result;
        if (!account.nameLength) return result;
        auto nameSize = static_cast<std::size_t>(account.nameLength) + 1;
        auto buf = std::make_unique<char[]>(nameSize);
        if (account.nameLength > 15) {
            std::uintptr_t buffer = 0;
            std::memcpy(&buffer, account.nameData, sizeof buffer);
            if (!IsValidPtr(buffer)) return result;
            if (!Rpm(buffer, buf.get(), nameSize)) return result;
        } else {
            std::memcpy(buf.get(), account.nameData, nameSize);
        }
        if (std::strlen(buf.get()) != static_cast<std::size_t>(account.nameLength)) return result;
        result = buf.get();
        return result;
    }
}