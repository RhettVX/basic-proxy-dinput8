#include <windows.h>
// TODO: We can safely removes some of the std includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#define HOOK_IMPLEMENTATION
#include "hook.h"
#define OUR_FUNCTIONS_IMPLEMENTATION
#include "our_functions.h"
#include "target_functions.h"


HMODULE moduleBase = GetModuleHandle("planetside2_x64.exe");

typedef HRESULT (*DirectInput8Create_t)(HINSTANCE  hinst,
                                        DWORD      dwVersion,
                                        REFIID     riidltf,
                                        LPVOID    *ppvOut,
                                        LPUNKNOWN  punkOuter);

static DirectInput8Create_t OriginalFunction = 0;
static HMODULE origDll = 0;

extern "C"
{
    __declspec(dllexport) HRESULT
    DirectInput8Create(HINSTANCE  hinst,
                       DWORD      dwVersion,
                       REFIID     riidltf,
                       LPVOID    *ppvOut,
                       LPUNKNOWN  punkOuter)
    {
        if(OriginalFunction)
        {
            uintptr_t target = 0x315d0;
            // TODO: This needed a bitwise OR instead of just a + for some reason. should investigate sometime
            // printf("OriginalFunction called.\nModuleBase: %p\nTarget: %p\nTargetAddr should be: %p\n\n", moduleBase, (void *)target, (void *)((uintptr_t)moduleBase | target));
            printf("OriginalFunction called.\nModuleBase: %p\n\n", moduleBase);
            // system("pause"); // TODO: remove this for real use
            TrampolineHook64((void *)((uintptr_t)moduleBase | target), (void *)TestFunc, 13);
            // system("pause"); // TODO: remove this for real use
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
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
}

BOOL WINAPI
DllMain(HINSTANCE hinstDLL,
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
