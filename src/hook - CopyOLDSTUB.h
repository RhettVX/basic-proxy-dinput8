#ifndef HOOK_H
#define HOOK_H

#include <stdlib.h>

// bool
// Hook64(void *toHook, void *ourFunc, uint32_t length);

void *
TrampolineHook64(void *target, void *ourFunc, int targetLength);

// TODO: x86 hooks are untested!
// bool
// Hook86(void *toHook, void *ourFunc, uint32_t length);

// void *
// TrampolineHook86(void *toHook, void *ourFunc, uint32_t length);


#endif // HOOK_H

#ifdef HOOK_IMPLEMENTATION


// bool
// Hook64(void *toHook, void *ourFunc, uint32_t length)
// {

// }

void *
TrampolineHook64(void *target, void *ourFunc, int targetLength)
{	
	int minLength = 14;
	if (targetLength < minLength)
	{
		return 0;
	}

	uint8_t stub[] = {0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 		       // jmp qword ptr [$+6]
				      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // ptr

	// Allocate a space to hold the real code and our stub
	void *trampoline = VirtualAlloc(0,
								  targetLength + sizeof(stub),
								  MEM_COMMIT | MEM_RESERVE,
								  PAGE_EXECUTE_READWRITE);

	// Take control of target in memory
	DWORD oldProtect = 0;
	VirtualProtect(target, targetLength, PAGE_EXECUTE_READWRITE, &oldProtect);

	// Return to? Seems to be a pointer to after the original bytes
	printf("TargetAddr: %p\n", target);
	uint64_t returnAddr = (uint64_t)target + targetLength;
	printf("ReturnAddr: %p\n", (void *)returnAddr);

	// we should be at base + 000315e2

	// Trampoline
	CopyMemory(stub + 6, &returnAddr, 8); // Copy returnAddr to the ptr in our stub
	printf("Stored addr: ");
	for (int i = 0; i < sizeof(stub); ++i)
	{
		printf("%02x ", stub[i]);
	}
	printf("\n");

	CopyMemory(trampoline, target, targetLength); // Copy real code into our allocated buffer
	printf("Copied code: ");
	for (int i = 0; i < targetLength; ++i)
	{
		printf("%02x ", *((BYTE *)trampoline + i));
	}
	printf("\n");


	CopyMemory((void *)((uintptr_t)trampoline + targetLength), stub, sizeof(stub)); // copy our stub after the real code
	printf("Our allocated buffer with stub and real code: ");
	for (int i = 0; i < (targetLength + sizeof(stub)); ++i)
	{
		printf("%02x ", *((BYTE *)trampoline + i));
	}
	printf("\n");



	// Original
	CopyMemory(stub + 6, &ourFunc, 8); // copy our function address to our stub to inject
	CopyMemory(target, stub, sizeof(stub)); // copy our injection stub to our target code

	for (int i = minLength; i < targetLength; ++i)
	{
		*(BYTE *)((DWORD_PTR)target + i) = 0x90; // nop
	}

	VirtualProtect(target, targetLength, oldProtect, 0);
	system("PAUSE");
	return trampoline;
}

// bool
// Hook86(void *toHook, void *ourFunc, uint32_t length)
// {
// 	if (length < 5)
// 	{
// 		return false;
// 	}

// 	DWORD originalProtection;
// 	VirtualProtect(toHook, length, PAGE_EXECUTE_READWRITE, &originalProtection);

// 	uintptr_t relativeAddress = (uintptr_t)ourFunc - (uintptr_t)toHook - 5;
// 	*(char *)toHook = 0xe9; // Relative jump
// 	*(uintptr_t *)((uintptr_t)toHook + 1) = relativeAddress;

// 	VirtualProtect(toHook, length, originalProtection, 0);
// 	return true;
// }

// void *
// TrampolineHook86(void *toHook, void *ourFunc, uint32_t length)
// {
// 	if (length < 5)
// 	{
// 		return 0;
// 	}

// 	void *gateway = VirtualAlloc(0, 
// 								 length + 5,
// 								 MEM_COMMIT | MEM_RESERVE,
// 								 PAGE_EXECUTE_READWRITE);

// 	CopyMemory(gateway, toHook, length);

// 	uintptr_t gatewayRelativeAddress = (uintptr_t)toHook - (uintptr_t)gateway - 5;
// 	*(char *)((uintptr_t)gateway + length) = 0xe9; // Relative jump
// 	*(uintptr_t *)((uintptr_t)gateway + length + 1) = gatewayRelativeAddress;

// 	Hook86(toHook, ourFunc, length);

// 	return gateway;
// }


#endif // HOOK_IMPLEMENTATION