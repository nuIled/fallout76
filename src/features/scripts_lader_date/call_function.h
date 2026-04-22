#pragma once

#include <cstdint>
#include <cstring>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

namespace CallFunction {

inline constexpr short  kStringParamLength   = 0x7E;
inline constexpr int    kStringOptionalCount = 10;
inline constexpr std::uintptr_t kOffsetCallFunction      = 0x00AC7CB0;
inline constexpr std::uintptr_t kOffsetString1StrOpt10   = 0x05A0F900;
inline constexpr std::uintptr_t kOffsetScriptConstructor = 0x00A7B3D0;

struct StringParam {
    short length;
    char  string[kStringParamLength];
};

struct StringOptionalParam {
    short length;
    char  string[kStringParamLength];
};

struct String1StringOptional10Format {
    char                padding[0x8];
    short               paramCount;
    StringParam         stringParam;
    StringOptionalParam stringOptionalParam[kStringOptionalCount];
};

struct Script {
    char padding[0x88];
};

[[nodiscard]] inline bool isValidPointer(std::uintptr_t ptr) noexcept {
    return ptr >= 0x10000ULL && ptr <= 0xF000000000000ULL && (ptr & 1) == 0;
}
[[nodiscard]] inline bool areScriptOffsetsValid() noexcept {
    return kOffsetCallFunction != 0 &&
           kOffsetString1StrOpt10 != 0 &&
           kOffsetScriptConstructor != 0;
}

void* initializeScript(std::uintptr_t baseAddress, Script* script);

void sendConsoleCommand(std::uintptr_t baseAddress,
                        std::uintptr_t cmdAddress,
                        std::uintptr_t fmtOffset,
                        void* fmtParams, void* target,
                        std::uintptr_t unknown,
                        std::uintptr_t offsetA, std::uintptr_t offsetB);

void callFunctionInternal(std::uintptr_t baseAddress,
                          void* fmtParams, void* target);

void fillStringParam(String1StringOptional10Format& fmt,
                     short paramCount,
                     const char* functionName,
                     const char* const* optionalParams,
                     int optionalCount);

[[nodiscard]] bool setPosition(std::uintptr_t baseAddress, void* target,
                               float x, float y, float z, float yaw);

[[nodiscard]] bool applyCombatSpell(std::uintptr_t baseAddress, void* target,
                                    std::uint32_t spellFormId);

[[nodiscard]] bool equipItem(std::uintptr_t baseAddress, void* target,
                             std::uint32_t formId);

[[nodiscard]] bool centerOnCell(std::uintptr_t baseAddress,
                                const char* cellEditorId);

[[nodiscard]] bool enableAI(std::uintptr_t baseAddress, void* target,
                            bool enable, bool pauseVoice);
[[nodiscard]] bool addPerk(std::uintptr_t baseAddress, void* target,
                           std::uint32_t perkFormId, bool notify);
[[nodiscard]] bool addSpell(std::uintptr_t baseAddress, void* target,
                            std::uint32_t spellFormId, bool verbose);
[[nodiscard]] bool killActor(std::uintptr_t baseAddress, void* target);
[[nodiscard]] bool playIdle(std::uintptr_t baseAddress, void* target,
                            std::uint32_t idleFormId);
[[nodiscard]] bool setAlpha(std::uintptr_t baseAddress, void* target, float alpha);
[[nodiscard]] bool setGhost(std::uintptr_t baseAddress, void* target, bool ghost);
[[nodiscard]] bool setActorValue(std::uintptr_t baseAddress, void* target,
                                 const char* avName, float value);
[[nodiscard]] bool rewardPlayerXP(std::uintptr_t baseAddress, std::uint32_t amount);
[[nodiscard]] bool setInChargen(std::uintptr_t baseAddress, bool inChargen);
[[nodiscard]] bool callPapyrusFunction(std::uintptr_t baseAddress,
                                       const char* functionName,
                                       void* target,
                                       const char* const* params,
                                       int paramCount);

}