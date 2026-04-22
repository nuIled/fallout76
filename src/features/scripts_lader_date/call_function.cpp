#include "call_function.h"

#include "../../core/sig_main/sig.h"
#include <cstdio>
#include <cstring>

namespace CallFunction {

static std::uintptr_t resolveCallFunctionAddress(std::uintptr_t baseAddress) {
    static std::uintptr_t cached = 0;
    if (cached) return cached;

    // From message.txt:
    // "ScriptObject.CallFunction":
    //   "sig": "48 8B C4 55 56 57 48 81 EC 90 00 00 00 48 C7 40 88 FE FF FF FF"
    const std::uintptr_t sigAddr =
        sigMain("48 8B C4 55 56 57 48 81 EC 90 00 00 00 48 C7 40 88 FE FF FF FF", 0, -1, 0);

    cached = sigAddr ? sigAddr : (baseAddress + kOffsetCallFunction);
    return cached;
}

void* initializeScript(std::uintptr_t baseAddress, Script* script) {
    if (!script)
        return nullptr;

    __try {
        using InitializeScriptFn = void*(__fastcall*)(Script*);
        auto fn = reinterpret_cast<InitializeScriptFn>(baseAddress + kOffsetScriptConstructor);
        return fn(script);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }
}

void sendConsoleCommand(std::uintptr_t baseAddress,
                        std::uintptr_t cmdAddress,
                        std::uintptr_t fmtOffset,
                        void* fmtParams, void* target,
                        std::uintptr_t unknown,
                        std::uintptr_t offsetA, std::uintptr_t offsetB) {
    Script scriptCommand{};
    void* pScriptCommand = initializeScript(baseAddress, &scriptCommand);
    if (!pScriptCommand)
        return;

    void** ppScriptCommand = &pScriptCommand;
    auto commandFormat  = fmtOffset ? baseAddress + fmtOffset : std::uintptr_t{0};
    auto commandOffsetA = offsetA;
    auto commandOffsetB = offsetB;

    __try {
        using DispatchFn = void(__fastcall*)(std::uintptr_t, void*, void*,
                                            std::uintptr_t, void*, void*, void*, void*);
        auto fn = reinterpret_cast<DispatchFn>(cmdAddress);
        fn(commandFormat, fmtParams, target, unknown,
           pScriptCommand, ppScriptCommand, &commandOffsetA, &commandOffsetB);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

void callFunctionInternal(std::uintptr_t baseAddress, void* fmtParams, void* target) {
    const std::uintptr_t callFn = resolveCallFunctionAddress(baseAddress);
    sendConsoleCommand(baseAddress, callFn, kOffsetString1StrOpt10,
                       fmtParams, target, 0, 8, 8);
}

void fillStringParam(String1StringOptional10Format& fmt,
                     short paramCount,
                     const char* functionName,
                     const char* const* optionalParams,
                     int optionalCount) {
    std::memset(&fmt, 0, sizeof(fmt));
    fmt.paramCount = paramCount;

    fmt.stringParam.length = kStringParamLength;
    strncpy_s(fmt.stringParam.string, functionName, kStringParamLength - 1);

    for (int i = 0; i < optionalCount && i < kStringOptionalCount; ++i) {
        fmt.stringOptionalParam[i].length = kStringParamLength;
        strncpy_s(fmt.stringOptionalParam[i].string,
                  optionalParams[i], kStringParamLength - 1);
    }
}

bool setPosition(std::uintptr_t baseAddress, void* target,
                 float x, float y, float z, float yaw) {
    if (!areScriptOffsetsValid() || !isValidPointer(reinterpret_cast<std::uintptr_t>(target)))
        return false;

    char xBuf[32], yBuf[32], zBuf[32], yawBuf[32];
    std::snprintf(xBuf,   sizeof(xBuf),   "%.6f", static_cast<double>(x));
    std::snprintf(yBuf,   sizeof(yBuf),   "%.6f", static_cast<double>(y));
    std::snprintf(zBuf,   sizeof(zBuf),   "%.6f", static_cast<double>(z));
    std::snprintf(yawBuf, sizeof(yawBuf), "%.6f", static_cast<double>(yaw));

    {
        const char* posParams[] = { xBuf, yBuf, zBuf };
        String1StringOptional10Format format{};
        fillStringParam(format, 4, "Actor.SetPosition", posParams, 3);
        callFunctionInternal(baseAddress, &format, target);
    }

    {
        const char* angleParams[] = { "0.000000", "0.000000", yawBuf };
        String1StringOptional10Format format{};
        fillStringParam(format, 4, "Actor.SetAngle", angleParams, 3);
        callFunctionInternal(baseAddress, &format, target);
    }

    return true;
}

bool applyCombatSpell(std::uintptr_t baseAddress, void* target,
                      std::uint32_t spellFormId) {
    if (!areScriptOffsetsValid() || !isValidPointer(reinterpret_cast<std::uintptr_t>(target)))
        return false;

    char formIdBuf[16];
    std::snprintf(formIdBuf, sizeof(formIdBuf), "%08X", spellFormId);

    const char* params[] = { formIdBuf, "Player" };
    String1StringOptional10Format format{};
    fillStringParam(format, 3, "Actor.DoCombatSpellApply", params, 2);
    callFunctionInternal(baseAddress, &format, target);
    return true;
}

bool equipItem(std::uintptr_t baseAddress, void* target, std::uint32_t formId) {
    if (!areScriptOffsetsValid() || !isValidPointer(reinterpret_cast<std::uintptr_t>(target)))
        return false;

    if (!formId)
        return false;

    char formIdBuf[16];
    std::snprintf(formIdBuf, sizeof(formIdBuf), "%08X", formId);

    const char* params[] = { formIdBuf };
    String1StringOptional10Format format{};
    fillStringParam(format, 2, "Actor.EquipItem", params, 1);
    callFunctionInternal(baseAddress, &format, target);
    return true;
}

bool centerOnCell(std::uintptr_t baseAddress, const char* cellEditorId) {
    if (!areScriptOffsetsValid() || !cellEditorId || !cellEditorId[0])
        return false;

    const char* params[] = { cellEditorId };
    String1StringOptional10Format format{};
    fillStringParam(format, 2, "Game.CenterOnCell", params, 1);
    callFunctionInternal(baseAddress, &format, nullptr);
    return true;
}

bool enableAI(std::uintptr_t baseAddress, void* target, bool enable, bool pauseVoice) {
    if (!areScriptOffsetsValid() || !isValidPointer(reinterpret_cast<std::uintptr_t>(target)))
        return false;

    const char* params[] = { enable ? "true" : "false", pauseVoice ? "true" : "false" };
    String1StringOptional10Format format{};
    fillStringParam(format, 3, "Actor.EnableAI", params, 2);
    callFunctionInternal(baseAddress, &format, target);
    return true;
}

bool evaluatePackage(std::uintptr_t baseAddress, void* target, bool resetAI) {
    if (!areScriptOffsetsValid() || !isValidPointer(reinterpret_cast<std::uintptr_t>(target)))
        return false;

    const char* params[] = { resetAI ? "true" : "false" };
    String1StringOptional10Format format{};
    fillStringParam(format, 2, "Actor.EvaluatePackage", params, 1);
    callFunctionInternal(baseAddress, &format, target);
    return true;
}

bool addPerk(std::uintptr_t baseAddress, void* target, std::uint32_t perkFormId, bool notify) {
    if (!areScriptOffsetsValid() || !isValidPointer(reinterpret_cast<std::uintptr_t>(target)))
        return false;

    char formIdBuf[16];
    std::snprintf(formIdBuf, sizeof(formIdBuf), "%08X", perkFormId);
    const char* params[] = { formIdBuf, notify ? "true" : "false" };
    String1StringOptional10Format format{};
    fillStringParam(format, 3, "Actor.AddPerk", params, 2);
    callFunctionInternal(baseAddress, &format, target);
    return true;
}

bool addSpell(std::uintptr_t baseAddress, void* target, std::uint32_t spellFormId, bool verbose) {
    if (!areScriptOffsetsValid() || !isValidPointer(reinterpret_cast<std::uintptr_t>(target)))
        return false;

    char formIdBuf[16];
    std::snprintf(formIdBuf, sizeof(formIdBuf), "%08X", spellFormId);
    const char* params[] = { formIdBuf, verbose ? "true" : "false" };
    String1StringOptional10Format format{};
    fillStringParam(format, 3, "Actor.AddSpell", params, 2);
    callFunctionInternal(baseAddress, &format, target);
    return true;
}

bool killActor(std::uintptr_t baseAddress, void* target) {
    if (!areScriptOffsetsValid() || !isValidPointer(reinterpret_cast<std::uintptr_t>(target)))
        return false;

    String1StringOptional10Format format{};
    fillStringParam(format, 1, "Actor.Kill", nullptr, 0);
    callFunctionInternal(baseAddress, &format, target);
    return true;
}

bool playIdle(std::uintptr_t baseAddress, void* target, std::uint32_t idleFormId) {
    if (!areScriptOffsetsValid() || !isValidPointer(reinterpret_cast<std::uintptr_t>(target)))
        return false;

    char formIdBuf[16];
    std::snprintf(formIdBuf, sizeof(formIdBuf), "%08X", idleFormId);
    const char* params[] = { formIdBuf };
    String1StringOptional10Format format{};
    fillStringParam(format, 2, "Actor.PlayIdle", params, 1);
    callFunctionInternal(baseAddress, &format, target);
    return true;
}

bool setAlpha(std::uintptr_t baseAddress, void* target, float alpha) {
    if (!areScriptOffsetsValid() || !isValidPointer(reinterpret_cast<std::uintptr_t>(target)))
        return false;

    char alphaBuf[32];
    std::snprintf(alphaBuf, sizeof(alphaBuf), "%.6f", static_cast<double>(alpha));
    const char* params[] = { alphaBuf, "false" };
    String1StringOptional10Format format{};
    fillStringParam(format, 3, "Actor.SetAlpha", params, 2);
    callFunctionInternal(baseAddress, &format, target);
    return true;
}

bool setGhost(std::uintptr_t baseAddress, void* target, bool ghost) {
    if (!areScriptOffsetsValid() || !isValidPointer(reinterpret_cast<std::uintptr_t>(target)))
        return false;

    const char* params[] = { ghost ? "true" : "false" };
    String1StringOptional10Format format{};
    fillStringParam(format, 2, "Actor.SetGhost", params, 1);
    callFunctionInternal(baseAddress, &format, target);
    return true;
}

bool setActorValue(std::uintptr_t baseAddress, void* target, const char* avName, float value) {
    if (!areScriptOffsetsValid() || !isValidPointer(reinterpret_cast<std::uintptr_t>(target)) || !avName)
        return false;

    char valueBuf[32];
    std::snprintf(valueBuf, sizeof(valueBuf), "%.6f", static_cast<double>(value));
    const char* params[] = { avName, valueBuf };
    String1StringOptional10Format format{};
    fillStringParam(format, 3, "Actor.SetAV", params, 2);
    callFunctionInternal(baseAddress, &format, target);
    return true;
}

bool rewardPlayerXP(std::uintptr_t baseAddress, std::uint32_t amount) {
    if (!areScriptOffsetsValid())
        return false;

    char xpBuf[32];
    std::snprintf(xpBuf, sizeof(xpBuf), "%u", amount);
    const char* params[] = { xpBuf };
    String1StringOptional10Format format{};
    fillStringParam(format, 2, "Game.RewardPlayerXP", params, 1);
    callFunctionInternal(baseAddress, &format, nullptr);
    return true;
}

bool setInChargen(std::uintptr_t baseAddress, bool inChargen) {
    if (!areScriptOffsetsValid())
        return false;

    const char* params[] = { inChargen ? "true" : "false" };
    String1StringOptional10Format format{};
    fillStringParam(format, 2, "Game.SetInChargen", params, 1);
    callFunctionInternal(baseAddress, &format, nullptr);
    return true;
}

bool callPapyrusFunction(std::uintptr_t baseAddress,
                         const char* functionName,
                         void* target,
                         const char* const* params,
                         int paramCount) {
    if (!areScriptOffsetsValid() || !functionName)
        return false;

    if (paramCount < 0) paramCount = 0;
    if (paramCount > kStringOptionalCount) paramCount = kStringOptionalCount;

    String1StringOptional10Format format{};
    fillStringParam(format, static_cast<short>(1 + paramCount),
                    functionName,
                    params,
                    paramCount);
    callFunctionInternal(baseAddress, &format, target);
    return true;
}

}