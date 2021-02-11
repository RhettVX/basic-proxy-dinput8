#include <windows.h>


typedef HRESULT (*DirectInput8Create_t)
(
    HINSTANCE  hinst,
    DWORD      dwVersion,
    REFIID     riidltf,
    LPVOID    *ppvOut,
    LPUNKNOWN  punkOuter
);

static DirectInput8Create_t OriginalFunction = 0;
static HMODULE origDll = 0;

extern "C"
{
    __declspec(dllexport) HRESULT
    DirectInput8Create(
        HINSTANCE  hinst,
        DWORD      dwVersion,
        REFIID     riidltf,
        LPVOID    *ppvOut,
        LPUNKNOWN  punkOuter)
    {
        if(OriginalFunction)
        {
            OutputDebugString("OriginalFunction called.\n");
            return OriginalFunction(hinst, dwVersion, riidltf, ppvOut, punkOuter);
        }
        MessageBox(0, "OriginalFunction is null.", 0, MB_OK);
        return 0;
    }
}


static void
Init()
{
    char dllPath[MAX_PATH] = {};
    GetSystemDirectory(dllPath, MAX_PATH);
    strcat(dllPath, "\\dinput8.dll");
    origDll = LoadLibrary(dllPath);

    if (origDll)
    {
        OriginalFunction = (DirectInput8Create_t)GetProcAddress(origDll, "DirectInput8Create");
    }
    else
    {
        MessageBox(0, "OriginalDll is null.", 0, MB_OK);
    }

    AllocConsole();
}

BOOL WINAPI
DllMain(
    HINSTANCE hinstDLL,
    DWORD     fdwReason,
    LPVOID    lpReserved)
{
    switch(fdwReason) 
    { 
    case DLL_PROCESS_ATTACH:
        Init();
    // case DLL_THREAD_ATTACH:
    // case DLL_THREAD_DETACH:
    // case DLL_PROCESS_DETACH:
    //     break;
    }
    return TRUE;
}
