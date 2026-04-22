#pragma once
#include <cstdint>
uintptr_t sigScan(const char* pattern, uintptr_t moduleBase = 0, size_t searchSize = 0);
uintptr_t sigResolver(uintptr_t matchAddr, int ripOffset);
uintptr_t sigMain(const char* pattern, int addOffset, int ripOffset = -1, int extraOffset = 0);