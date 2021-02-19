#ifndef HOOK_H
#define HOOK_H

// FIXME: This crashes randomly for different reasons


#define MIN_LENGTH 12

void *
TrampolineHook64(void *target, void *ourFunc, int targetLength);


#endif // HOOK_H

#ifdef HOOK_IMPLEMENTATION


void *
TrampolineHook64(void *target, void *ourFunc, int targetLength)
{   
    printf("[*] Attempting trampoline hook.\n");

    printf("target: %#p\nourFunc: %#p\ntargetLength: %d\nreturnAddr: %#p\n",
           target,
           ourFunc,
           targetLength,
           (void *)((uintptr_t)target + targetLength));

    if (!target || !ourFunc || targetLength < MIN_LENGTH)
    {
        return 0;
    }

    printf("\nOriginal code:\n");
    for (int i = 0; i < targetLength; ++i)
    {
        printf("%02x", *((uint8_t *)target + i));
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

    uint8_t callStub[] = {0x49, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //mov
                          0x41, 0xFF, 0xD3}; // call
    *(uintptr_t *)(callStub + 2) = (uintptr_t)ourFunc;

    void *trampoline = VirtualAlloc(0, 
                                    targetLength + sizeof(callStub) + MIN_LENGTH,
                                    MEM_COMMIT | MEM_RESERVE,
                                    PAGE_EXECUTE_READWRITE);
    if (!trampoline)
    {
        return 0;
    }

    printf("trampoline: %#p\n", trampoline);
    // We should be able to use rax and r11
    CopyMemory(trampoline, target, targetLength);
    uint8_t codeCave[MIN_LENGTH] = {0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // + 0x00 (+ 0x02)  -> mov rax, pHook
                                    0xFF, 0xE0};   
    *(uintptr_t *)(codeCave + 2) = (uintptr_t)target + targetLength;



    CopyMemory((uint8_t *)trampoline + targetLength, callStub, sizeof(callStub));
    CopyMemory((uint8_t *)trampoline + targetLength + sizeof(callStub), codeCave, sizeof(codeCave));


    printf("\nTrampoline code:\n");
    for (int i = 0; i < (targetLength + MIN_LENGTH); ++i)
    {
        printf("%02x", *((uint8_t *)trampoline + i));
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


    printf("\nCallStub code:\n");
    for (int i = 0; i < sizeof(callStub); ++i)
    {
        printf("%02x", *((uint8_t *)callStub + i));
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


    *(uintptr_t *)(codeCave + 2) = (uintptr_t)trampoline;

    DWORD oldProtect = 0;
    if (!VirtualProtect(target, sizeof(codeCave), PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        return 0;
    }

    FillMemory(target, targetLength, 0x90);
    CopyMemory(target, codeCave, sizeof(codeCave));

    printf("\nModified code:\n");
    for (int i = 0; i < sizeof(codeCave); ++i)
    {
        printf("%02x", *((uint8_t *)target + i));
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

    VirtualProtect(target, sizeof(codeCave), oldProtect, &oldProtect);

    printf("\n[*] Trampoline hook should be finished.\n");
    return trampoline;
}


#endif // HOOK_IMPLEMENTATION