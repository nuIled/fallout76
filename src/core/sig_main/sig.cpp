#include "sig.h"
#include "../dll_main/globals.h"
#include <Windows.h>
#include <vector>
#include <cstring>
#include <cstdio>

static uintptr_t getBaseSize(uintptr_t* outSize) {
    uintptr_t base = getbase();
    if (!base || !outSize) return base;
    __try {
        auto* dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
            *outSize = 0x2000000;
            return base;
        }
        auto* ntHeaders = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dosHeader->e_lfanew);
        if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
            *outSize = 0x2000000;
            return base;
        }
        *outSize = ntHeaders->OptionalHeader.SizeOfImage;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        *outSize = 0x2000000;
    }
    return base;
}

static bool parsePattern(const char* pattern, std::vector<uint8_t>& bytes, std::vector<uint8_t>& mask) {
    bytes.clear();
    mask.clear();
    const char* p = pattern;
    while (*p) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;
        if (p[0] == '?') {
            bytes.push_back(0);
            mask.push_back(0);
            p += (p[1] == '?') ? 2 : 1;
            continue;
        }
        unsigned int v = 0;
        if (sscanf_s(p, "%02X", &v) != 1) return false;
        bytes.push_back(static_cast<uint8_t>(v));
        mask.push_back(1);
        p += 2;
    }
    return !bytes.empty();
}

uintptr_t sigScan(const char* pattern, uintptr_t moduleBase, size_t searchSize) {
    std::vector<uint8_t> bytes, mask;
    if (!parsePattern(pattern, bytes, mask)) return 0;
    uintptr_t sizeVal = searchSize;
    if (!moduleBase || !sizeVal) {
        moduleBase = getBaseSize(&sizeVal);
    }
    if (!moduleBase || sizeVal == 0) return 0;

    const uint8_t* data = reinterpret_cast<const uint8_t*>(moduleBase);
    const size_t n = bytes.size();

    for (size_t i = 0; i + n <= sizeVal; i++) {
        bool match = true;
        for (size_t j = 0; j < n; j++) {
            if (mask[j] && data[i + j] != bytes[j]) {
                match = false;
                break;
            }
        }
        if (match) return moduleBase + i;
    }
    return 0;
}

uintptr_t sigResolver(uintptr_t matchAddr, int ripOffset) {
    __try {
        int32_t disp = *reinterpret_cast<int32_t*>(matchAddr + ripOffset);
        return (matchAddr + ripOffset + 4) + disp;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
}

uintptr_t sigMain(const char* pattern, int addOffset, int ripOffset, int extraOffset) {
    uintptr_t size = 0;
    getBaseSize(&size);
    uintptr_t match = sigScan(pattern, 0, size);
    if (!match) return 0;
    match += addOffset;
    if (ripOffset >= 0) {
        match = sigResolver(match, ripOffset);
        if (!match) return 0;
    }
    return match + extraOffset;
}