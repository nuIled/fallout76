#include "globals.h"
#include "../../gui/menu_main/gui.h"
#include <thread>
#include <exception>
#include <windows.h>
#include <shlwapi.h>
#include <fstream>
#include <sstream>

#pragma comment(lib, "Shlwapi.lib")

uintptr_t uiViewAngleOffset = 0;
bool bCritsDisabled = false;
bool bCritsEnabled = false;
bool bAmmoDisabled = false;
float fCurViewPitch = 0.0f;
float fCurViewYaw = 0.0f;
float fNoclipSpeed = 1.0f;
bool bNoclipEnabled = false;
bool bVelDisabled = false;
bool bVelEnabled = false;
float fVelValue = 0.0f;
bool bVelFrozen = false;
bool bVelNoclipFrozen = false;

static void removeDMPS(HMODULE hModule) {
    char dllPath[MAX_PATH];
    GetModuleFileNameA(hModule, dllPath, MAX_PATH);
    PathRemoveFileSpecA(dllPath);
    
    char searchPath[MAX_PATH];
    snprintf(searchPath, MAX_PATH, "%s\\*.dmp", dllPath);
    
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath, &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            char filePath[MAX_PATH];
            snprintf(filePath, MAX_PATH, "%s\\%s", dllPath, findData.cFileName);
            DeleteFileA(filePath);
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
}
static void deleteCrashSender(HMODULE hModule) {
    char dllPath[MAX_PATH];
    GetModuleFileNameA(hModule, dllPath, MAX_PATH);
    PathRemoveFileSpecA(dllPath);

    char exePath[MAX_PATH];
    snprintf(exePath, MAX_PATH, "%s\\CrashSender.exe", dllPath);// deprecated, recent patch broke this...

    if (DeleteFileA(exePath)) {
    }
}

static void wipetracking() {
    char* docsPath = nullptr;
    size_t len = 0;
    if (_dupenv_s(&docsPath, &len, "USERPROFILE") != 0 || !docsPath) {
        return;
    }

    std::string iniPath = std::string(docsPath) + "\\Documents\\My Games\\Fallout 76\\Fallout76Prefs.ini";
    free(docsPath);

    std::ifstream inFile(iniPath);
    if (!inFile.is_open()) {
        return;
    }

    std::ostringstream newContent;
    std::string line;
    bool inBethesdaSection = false;
    
    while (std::getline(inFile, line)) {
        if (line == "[Bethesda.net]") {
            inBethesdaSection = true;
            continue;
        }
        
        if (inBethesdaSection) {
            if (!line.empty() && line[0] == '[') {
                inBethesdaSection = false;
                newContent << line << "\n";
            } else if (
                line.find("uPersistentUuidData0=") == 0 ||
                line.find("uPersistentUuidData1=") == 0 ||
                line.find("uPersistentUuidData2=") == 0 ||
                line.find("uPersistentUuidData3=") == 0
            ) {
                continue;
            }
        } else {
            newContent << line << "\n";
        }
    }
    inFile.close();
    
    std::ofstream outFile(iniPath, std::ios::trunc);
    if (outFile.is_open()) {
        outFile << newContent.str();
        outFile.close();
    }
}

static void initialize() {
    try {
        loadD3D11Functions();
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        std::thread(noclipLoop).detach();
        std::thread(freecamLoop).detach();
        std::thread(applyVel).detach();
        std::thread(setcrits).detach();
        std::thread(setPlayerSpeed).detach();
        std::thread(runSprintOverweightLoop).detach();
        std::thread(applystate).detach();
        std::thread(runesmpatcher).detach();
        std::thread(runomodpatcher).detach();
        std::thread([]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            applyMods();//i think apply this at start is what i was supposed to do originally, works now
        }).detach();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
    } catch (const std::exception& e) {
        std::string errorMsg = "init error: ";
        errorMsg += e.what();
        MessageBoxA(nullptr, errorMsg.c_str(), "ilydealer", MB_OK | MB_ICONERROR);
    } catch (...) {
        MessageBoxA(nullptr, "unknown init error", "ilydealer", MB_OK | MB_ICONERROR);
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    UNREFERENCED_PARAMETER(lpReserved);
    
    static bool cleaned = false;
    
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            if (!cleaned) {
                removeDMPS(hinstDLL);
                deleteCrashSender(hinstDLL);
                wipetracking();
                cleaned = true;
                std::thread([]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(6000));
                }).detach();
            }
            std::thread(initialize).detach();
            break;
            
        case DLL_PROCESS_DETACH:
            bCritsDisabled = true;
            bAmmoDisabled = true;
            bVelDisabled = true;
            bSprintFatDisabled = true;
            bAmmoStop = true;
            break;
    }
    return TRUE;
}