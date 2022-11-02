#pragma warning(disable: 4505)  // unreferenced function with internal linkage has been removed


#include <stdint.h>

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include "main.h"

#include "util.cpp"
#include "hook.cpp"
#include "endian.c"


#ifdef C_ADMIN
global char target_name[] = "YoteSide.exe";
#endif

#ifdef C_BETA
global char target_name[] = "PlanetSide2_patched.exe";
#endif

#ifdef C_TARGET
global char target_name[] = "PlanetSide2_x86.exe";
#endif

#ifdef C_TARGET_3
global char target_name[] = "PlanetSide2_x64.exe";
#endif

global HMODULE target_module;
global HMODULE original_dll;


#define DIRECT_INPUT_CREATE(name)  HRESULT name(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
typedef DIRECT_INPUT_CREATE(DirectInput8Create_t);

global DirectInput8Create_t* DirectInput8Create_original;
global HANDLE global_standard_handle;

#define LOG_TO_FILE_MAIN(name) void __cdecl name(int log_level, char* file_name, char* format_message, va_list args)
typedef LOG_TO_FILE_MAIN(log_to_file_main_t);

log_to_file_main_t* log_to_file_main_original = nullptr;

internal LOG_TO_FILE_MAIN(log_to_file_main_ovr)
{
	char original_message_buffer[512] = {0};
	vsnprintf(original_message_buffer, sizeof(original_message_buffer), format_message, args);

	SYSTEMTIME log_time = { 0 };
	GetLocalTime(&log_time);

	char log_message_buffer[1024] = {0};
	sprintf(log_message_buffer,
	        #ifdef SHOW_HEX_LOGSTRING
	        "[%d-%02d-%02d %02d:%02d:%02d] (L%d) <%s:%p> %s\n",
	        #else
	        "[%d-%02d-%02d %02d:%02d:%02d] (L%d) <%s> %s\n",
	        #endif
	        log_time.wYear, log_time.wDay, log_time.wMonth,
	        log_time.wHour, log_time.wMinute, log_time.wSecond,
	        #ifdef SHOW_HEX_LOGSTRING
	        log_level, file_name, file_name, original_message_buffer);
#else
	log_level, file_name, original_message_buffer);
#endif
util_log_message(log_message_buffer, TRUE);
}

	dll_external DIRECT_INPUT_CREATE(DirectInput8Create)
	{
		if (DirectInput8Create_original)
		{
			local_persist b32 hooked;
			if (!hooked)
			{

#if defined(X86) && defined(C_TARGET)
				log_to_file_main_original = (log_to_file_main_t*)hook_x86_trampoline((void*)0x92e550,
				                                                                     0xc,
				                                                                     log_to_file_main_ovr);
#endif

#if defined(X64) && defined(C_TARGET_3)
				log_to_file_main_original = (log_to_file_main_t*)hook_x64_trampoline((void*)0x140038b90,
				                                                                     0x10,
				                                                                     log_to_file_main_ovr);
#endif

				hooked = 1;
			}

			return DirectInput8Create_original(hinst, dwVersion, riidltf, ppvOut, punkOuter);
		}

		MessageBox(0, "DirectInput8Create_func is null.", 0, MB_OK);
		return 0;
	}

	internal void init(HINSTANCE hinstDLL)
	{
		UNUSED(hinstDLL);

		AllocConsole();
		global_standard_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		freopen("CONOUT$", "w", stdout);

		target_module = GetModuleHandle(target_name);
		if (!target_module)
		{
			MessageBox(0, "[X] Target module address not found, make sure you remembered to update the target name!", 0, MB_OK);
			ExitProcess(1);
		}

		original_dll = LoadLibrary("C:\\WINDOWS\\system32\\dinput8.dll");
		if (!original_dll)
		{
			MessageBox(0, "OriginalDll is null.", 0, MB_OK);
			return;
		}

		DirectInput8Create_original = (DirectInput8Create_t*)GetProcAddress(original_dll, "DirectInput8Create");
		if (!DirectInput8Create_original)
		{
			MessageBox(0, "DirectInput8Create_func is null.", 0, MB_OK);
			return;
		}

#if defined(X86) && defined(C_BETA)
		log_to_file_main_original = (log_to_file_main_type)hook_x86_trampoline((void*)0x6f05f0,
		                                                                       0xc,
		                                                                       log_to_file_main_override);
#endif

	}

	BOOL WINAPI DllMain(HINSTANCE hinstDLL,
	                    DWORD fdwReason,
	                    LPVOID lpReserved)
	{
		UNUSED(lpReserved);

		switch (fdwReason)
		{
			case DLL_PROCESS_ATTACH:
				init(hinstDLL);
				break;

			// case DLL_THREAD_ATTACH:
			// case DLL_THREAD_DETACH:
			// case DLL_PROCESS_ATTACH:
		}

		return TRUE;
	}
