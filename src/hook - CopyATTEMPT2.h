#ifndef HOOK_H
#define HOOK_H

// FIXME: This crashes randomly for different reasons


// bool
// Hook64(void *toHook, void *ourFunc, uint32_t length);

void
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

void
TrampolineHook64(void *target, void *ourFunc, int targetLength)
{   
    printf("[*] Attempting trampoline hook.\n");
    int minLength = 14;
    if (targetLength < minLength)
    {
        return;
    }

    uint8_t redirectStub[] = {0x52,                                                       // push   rdx
                              0x48, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs rdx, PTR_TO_OUR_NEW_CODE ; assuming our ptr is: 0x0000000000000000
                              0xFF, 0xE2,                                                 // jmp    rdx
                              0x5A};                                                      // pop    rdx                      ; return original value to rdx

    uint8_t jumpStub[] =     {0x48, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs rdx, PTR_TO_OUR_ORIGINAL ; this is the addr of that 'pop rdx' instruction we injected
                              0xFF, 0xE2};                                                // jmp    rdx

    uint8_t callStub[] =     {0x48, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs rdx, 0x0000000000000000
                              0xFF, 0xD2};                                                // call   rdx

    printf("Target: %#p\n", target);
    printf("OurFunc: %#p\n", ourFunc);                                                
    // Allocate a space to hold the real code and our redirects and pops/pushes
    void *trampoline = VirtualAlloc(0,
                                    targetLength + sizeof(callStub) + sizeof(jumpStub) + 2,
                                    MEM_COMMIT | MEM_RESERVE,
                                    PAGE_EXECUTE_READWRITE);

    printf("Trampoline: %#p\n", trampoline);

    // Take control of target in memory
    DWORD oldProtect = 0;
    VirtualProtect(target, targetLength, PAGE_EXECUTE_READWRITE, &oldProtect);

    // Calculate address to return to
    uint64_t returnAddr = (uint64_t)target + targetLength - 1;
    printf("ReturnAddr: %#p\n", (void *)returnAddr);

    //// Trampoline
    // Copy our function pointer to a call stub
    CopyMemory(callStub + 2, &ourFunc, 8);

    // Copy the call to our function into the trampoline
    CopyMemory(trampoline, callStub, sizeof(callStub));

    // Pop rdx for use in the original code
    *(uint8_t *)((uintptr_t)trampoline + sizeof(callStub)) = 0x5A;

    // Copy the real code after our function call in the trampoline
    CopyMemory((void *)((uintptr_t)trampoline + sizeof(callStub) + 1), target, targetLength);

    // Push rdx so we can pop it again after we're done with it
    *(uint8_t *)((uintptr_t)trampoline + sizeof(callStub) + targetLength + 1) = 0x52;

    // Copy the return address to a jump stub
    CopyMemory(jumpStub + 2, &returnAddr, 8);

    // Copy the return jump after the real code in the trampoline
    CopyMemory((void *)((uintptr_t)trampoline + sizeof(callStub) + targetLength + 2), jumpStub, sizeof(jumpStub));

    printf("Trampoline contents:\n");
    for (int i = 0; i < (targetLength + sizeof(callStub) + sizeof(jumpStub) + 2); ++i)
    {
        printf("%02x", *((BYTE *)trampoline + i));
        // printf("%d\n", i);
        if ((i + 1) % 8 == 0)
        {
            printf("\n");
        }
        else
        {
            printf(" ");
        }
    }
    printf("\n");

    //// Original
    // Copy our trampoline address to our redirect stub
    CopyMemory(redirectStub + 3, &trampoline, 8);

    // Overwrite the target code with our redirect
    CopyMemory(target, redirectStub, sizeof(redirectStub));

    // NOP anything left behind
    for (int i = minLength; i < targetLength; ++i)
    {
        *(uint8_t *)((uintptr_t)target + i) = 0x90; // nop
    }

    // Restore protection
    VirtualProtect(target, targetLength, oldProtect, &oldProtect);

    printf("[*] Trampoline hook should be finished.\n");
    return;
}

// bool
// Hook86(void *toHook, void *ourFunc, uint32_t length)
// {
//  if (length < 5)
//  {
//      return false;
//  }

//  DWORD originalProtection;
//  VirtualProtect(toHook, length, PAGE_EXECUTE_READWRITE, &originalProtection);

//  uintptr_t relativeAddress = (uintptr_t)ourFunc - (uintptr_t)toHook - 5;
//  *(char *)toHook = 0xe9; // Relative jump
//  *(uintptr_t *)((uintptr_t)toHook + 1) = relativeAddress;

//  VirtualProtect(toHook, length, originalProtection, 0);
//  return true;
// }

// void *
// TrampolineHook86(void *toHook, void *ourFunc, uint32_t length)
// {
//  if (length < 5)
//  {
//      return 0;
//  }

//  void *gateway = VirtualAlloc(0, 
//                               length + 5,
//                               MEM_COMMIT | MEM_RESERVE,
//                               PAGE_EXECUTE_READWRITE);

//  CopyMemory(gateway, toHook, length);

//  uintptr_t gatewayRelativeAddress = (uintptr_t)toHook - (uintptr_t)gateway - 5;
//  *(char *)((uintptr_t)gateway + length) = 0xe9; // Relative jump
//  *(uintptr_t *)((uintptr_t)gateway + length + 1) = gatewayRelativeAddress;

//  Hook86(toHook, ourFunc, length);

//  return gateway;
// }


#endif // HOOK_IMPLEMENTATION