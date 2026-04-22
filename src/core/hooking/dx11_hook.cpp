#include "../dll_main/globals.h"
#include "../../features/esp/esp_main/esp.h"
#include "../../../libs/imgui/imgui.h"
#include "../../../libs/imgui/backends/imgui_impl_win32.h"
#include "../../../libs/imgui/backends/imgui_impl_dx11.h"
#include "../../../libs/minhook/MinHook.h"

static HRESULT(WINAPI* pD3D11CreateDevice)(
    IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, 
    const D3D_FEATURE_LEVEL*, UINT, UINT, 
    ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**
) = nullptr;

static HRESULT(WINAPI* pD3D11CreateDeviceAndSwapChain)(
    IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
    const D3D_FEATURE_LEVEL*, UINT, UINT, const DXGI_SWAP_CHAIN_DESC*,
    IDXGISwapChain**, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**
) = nullptr;

static HRESULT(WINAPI* pD3D11On12CreateDevice)(
    IUnknown*, UINT, const D3D_FEATURE_LEVEL*, UINT,
    IUnknown* const*, UINT, UINT, 
    ID3D11Device**, ID3D11DeviceContext**, D3D_FEATURE_LEVEL*
) = nullptr;

#pragma comment(linker, "/export:D3D11CreateDevice=ShimD3D11CreateDevice")
#pragma comment(linker, "/export:D3D11CreateDeviceAndSwapChain=ShimD3D11CreateDeviceAndSwapChain")
#pragma comment(linker, "/export:D3D11On12CreateDevice=ShimD3D11On12CreateDevice")

extern "C" {

HRESULT WINAPI ShimD3D11CreateDevice(
    IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext) 
{
    static volatile unsigned long long initCount = 0;

    if (InterlockedIncrement(&initCount) == 1) {
        loadD3D11Functions();
    }

    if (!pD3D11CreateDevice) {
        loadD3D11Functions();
        if (!pD3D11CreateDevice) {
            return E_FAIL;
        }
    }

    return pD3D11CreateDevice(
        pAdapter, DriverType, Software, Flags,
        pFeatureLevels, FeatureLevels, SDKVersion,
        ppDevice, pFeatureLevel, ppImmediateContext
    );
}

HRESULT WINAPI ShimD3D11CreateDeviceAndSwapChain(
    IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
    IDXGISwapChain** ppSwapChain,
    ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext) 
{
    if (!pD3D11CreateDeviceAndSwapChain) {
        loadD3D11Functions();
        if (!pD3D11CreateDeviceAndSwapChain) {
            return E_FAIL;
        }
    }

    HRESULT hr = pD3D11CreateDeviceAndSwapChain(
        pAdapter, DriverType, Software, Flags,
        pFeatureLevels, FeatureLevels, SDKVersion,
        pSwapChainDesc, ppSwapChain, ppDevice,
        pFeatureLevel, ppImmediateContext
    );

    if (SUCCEEDED(hr) && ppSwapChain && *ppSwapChain) {
        hookSwapChain(*ppSwapChain);
    }

    return hr;
}

HRESULT WINAPI ShimD3D11On12CreateDevice(
    IUnknown* pDevice,
    UINT Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    IUnknown* const* ppCommandQueues,
    UINT NumQueues,
    UINT NodeMask,
    ID3D11Device** ppDevice,
    ID3D11DeviceContext** ppImmediateContext,
    D3D_FEATURE_LEVEL* pChosenFeatureLevel) 
{
    if (!pD3D11On12CreateDevice) {
        loadD3D11Functions();
        if (!pD3D11On12CreateDevice) {
            return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
        }
    }

    return pD3D11On12CreateDevice(
        pDevice, Flags, pFeatureLevels, FeatureLevels,
        ppCommandQueues, NumQueues, NodeMask,
        ppDevice, ppImmediateContext, pChosenFeatureLevel
    );
}

}

void loadD3D11Functions() {
    char dllPath[MAX_PATH];
    ZeroMemory(dllPath, sizeof(dllPath));
    GetSystemDirectoryA(dllPath, sizeof(dllPath));
    strcat_s(dllPath, "\\d3d11.dll");

    HMODULE d3d11 = LoadLibraryA(dllPath);

    if (!d3d11) {
        MessageBoxA(nullptr, 
            "shiny error! if you manage to pull this, you are probably using linux! #archbtw", 
            "ilydealer", MB_OK | MB_ICONERROR);
        return;
    }

    pD3D11CreateDevice = reinterpret_cast<decltype(pD3D11CreateDevice)>(
        GetProcAddress(d3d11, "D3D11CreateDevice"));
    pD3D11CreateDeviceAndSwapChain = reinterpret_cast<decltype(pD3D11CreateDeviceAndSwapChain)>(
        GetProcAddress(d3d11, "D3D11CreateDeviceAndSwapChain"));
    pD3D11On12CreateDevice = reinterpret_cast<decltype(pD3D11On12CreateDevice)>(
        GetProcAddress(d3d11, "D3D11On12CreateDevice"));

    if (!pD3D11CreateDevice || !pD3D11CreateDeviceAndSwapChain) {
        MessageBoxA(nullptr, 
            "Failed to load required D3D11 exports - corrupt windows / not on windows", 
            "ilydealer", MB_OK | MB_ICONERROR);
    }
}

static bool g_ImGuiInitialized = false;
bool bDisplayoverlay = false;
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static HWND g_hWnd = nullptr;
static WNDPROC g_OriginalWndProc = nullptr;
static bool g_CursorWasVisible = false;

typedef HRESULT(STDMETHODCALLTYPE* PresentFn)(IDXGISwapChain*, UINT, UINT);
typedef HRESULT(STDMETHODCALLTYPE* ResizeBuffersFn)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

static PresentFn oPresent = nullptr;
static ResizeBuffersFn oResizeBuffers = nullptr;

static void initializeImGui(IDXGISwapChain* pSwapChain);
static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HRESULT STDMETHODCALLTYPE hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    if (!oPresent) {
        return E_FAIL;
    }

    if (!g_ImGuiInitialized && pSwapChain) {
        initializeImGui(pSwapChain);
    }

    if (g_ImGuiInitialized) {
        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX11_NewFrame();
        ImGui::NewFrame();
        renderMenu();
        ESP::UpdateESP();
        ESP::RenderESP();
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        bool shouldShowCursor = bDisplayoverlay && io.WantCaptureMouse;

        if (shouldShowCursor) {// this can fix overlay mismatch cursor bug!! wow!!
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            if (!g_CursorWasVisible) {
                int displayCount;
                do {
                    displayCount = ShowCursor(TRUE);
                } while (displayCount < 0);
                g_CursorWasVisible = true;
            }
        } else {
            if (g_CursorWasVisible) {
                int displayCount;
                do {
                    displayCount = ShowCursor(FALSE);
                } while (displayCount > 0);
                g_CursorWasVisible = false;
            }
        }
    }

    return oPresent(pSwapChain, SyncInterval, Flags);
}

HRESULT STDMETHODCALLTYPE hkResizeBuffers(
    IDXGISwapChain* pSwapChain, UINT BufferCount, 
    UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) 
{
    if (!oResizeBuffers) {
        return E_FAIL;
    }

    if (g_ImGuiInitialized) {
        ImGui_ImplDX11_InvalidateDeviceObjects();
    }

    HRESULT hr = oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

    if (g_ImGuiInitialized && SUCCEEDED(hr)) {
        ImGui_ImplDX11_CreateDeviceObjects();
    }

    return hr;
}

static void initializeImGui(IDXGISwapChain* pSwapChain) {
    if (g_ImGuiInitialized || !pSwapChain) {
        return;
    }

    g_pSwapChain = pSwapChain;
    g_pSwapChain->AddRef();

    if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pd3dDevice))) {
        return;
    }

    g_pd3dDevice->GetImmediateContext(&g_pd3dDeviceContext);
    
    DXGI_SWAP_CHAIN_DESC sd;
    pSwapChain->GetDesc(&sd);
    g_hWnd = sd.OutputWindow;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImFontConfig fontConfig;
    fontConfig.OversampleH = 3;
    fontConfig.OversampleV = 2;
    fontConfig.RasterizerMultiply = 1.0f;
    fontConfig.PixelSnapH = true;
    extern float ffontsize;
    float fontSize = (ffontsize >= 12.0f && ffontsize <= 24.0f) ? ffontsize : 20.0f;
    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", fontSize, &fontConfig);// change this at a later date
    if (!font) {
        font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", fontSize, &fontConfig);
        if (!font) {
            font = io.Fonts->AddFontDefault(&fontConfig);
        }
    }

    ImFontConfig espFontConfig = {};
    espFontConfig.OversampleH = 3;
    espFontConfig.OversampleV = 2;
    espFontConfig.PixelSnapH = true;
    espFontConfig.RasterizerMultiply = 1.05f;
    constexpr float kEspFontBakePx = 28.0f;
    if (!io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", kEspFontBakePx, &espFontConfig))
        io.Fonts->AddFontDefault(&espFontConfig);

    io.Fonts->Build();

    ImGuiStyle& style = ImGui::GetStyle();
    style.AntiAliasedLines = true;
    style.AntiAliasedLinesUseTex = true;
    style.AntiAliasedFill = true;
    style.WindowRounding = 4.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.ChildRounding = 3.0f;
    style.PopupRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.TabRounding = 3.0f;
    style.Alpha = 1.0f;
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;

    if (!ImGui_ImplWin32_Init(g_hWnd)) {
        ImGui::DestroyContext();
        return;
    }

    if (!ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext)) {
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        return;
    }

    g_OriginalWndProc = (WNDPROC)SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

    g_ImGuiInitialized = true;
}

void hookSwapChain(IDXGISwapChain* pSwapChain) {
    if (!pSwapChain || oPresent) {
        return;
    }

    void** pVTable = *(void***)pSwapChain;

    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
        return;
    }
    if (MH_CreateHook((LPVOID)pVTable[8], (LPVOID)hkPresent, (LPVOID*)&oPresent) == MH_OK) {
        MH_EnableHook((LPVOID)pVTable[8]);
    }
    if (MH_CreateHook((LPVOID)pVTable[13], (LPVOID)hkResizeBuffers, (LPVOID*)&oResizeBuffers) == MH_OK) {
        MH_EnableHook((LPVOID)pVTable[13]);
    }
}

static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_ImGuiInitialized) {
        if (msg == WM_KEYDOWN && wParam == VK_INSERT) {
            bDisplayoverlay = !bDisplayoverlay;

            if (bDisplayoverlay) {
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
                int displayCount;
                do {
                    displayCount = ShowCursor(TRUE);
                } while (displayCount < 0);
                g_CursorWasVisible = true;
            } else {
                int displayCount;
                do {
                    displayCount = ShowCursor(FALSE);
                } while (displayCount > 0);
                g_CursorWasVisible = false;
            }
            return 0;
        }

        ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
        ImGuiIO& io = ImGui::GetIO();

        if (msg == WM_SETCURSOR) {
            if (bDisplayoverlay && io.WantCaptureMouse) {
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
                if (!g_CursorWasVisible) {
                    int displayCount;
                    do {
                        displayCount = ShowCursor(TRUE);
                    } while (displayCount < 0);
                    g_CursorWasVisible = true;
                }
                return TRUE;
            } else if (!bDisplayoverlay && g_CursorWasVisible) {
                int displayCount;
                do {
                    displayCount = ShowCursor(FALSE);
                } while (displayCount > 0);
                g_CursorWasVisible = false;
            }
        }

        if (msg == WM_MOUSEMOVE) {
            bool shouldShowCursor = bDisplayoverlay && io.WantCaptureMouse;
            if (shouldShowCursor && !g_CursorWasVisible) {
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
                int displayCount;
                do {
                    displayCount = ShowCursor(TRUE);
                } while (displayCount < 0);
                g_CursorWasVisible = true;
            } else if (!shouldShowCursor && g_CursorWasVisible) {
                int displayCount;
                do {
                    displayCount = ShowCursor(FALSE);
                } while (displayCount > 0);
                g_CursorWasVisible = false;
            }
        }
        if (bDisplayoverlay) {
            if (io.WantCaptureMouse && (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST)) {
                return 0;
            }
            if (io.WantCaptureKeyboard && (msg >= WM_KEYFIRST && msg <= WM_KEYLAST)) {
                return 0;
            }
        }
    }

    if (g_OriginalWndProc) {
        return CallWindowProc(g_OriginalWndProc, hWnd, msg, wParam, lParam);
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}