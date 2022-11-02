#define hook_nop(p, l) hook_memory_set(p, 0x90, l)

internal void hook_memory_set(void* target, u8 value, i32 target_length)
{
	DWORD protection;
	VirtualProtect(target, target_length, PAGE_EXECUTE_READWRITE, &protection);
	util_memory_set((u8*)target, value, target_length);
	VirtualProtect(target, target_length, protection, &protection);
}

#ifdef X86
internal b32 hook_x86_detour(void* target, i32 target_length, void* our_function)
{
	if (target_length < 5)
	{
		return FALSE;
	}

	DWORD protection;
	VirtualProtect(target, target_length, PAGE_EXECUTE_READWRITE, &protection);

	util_memory_set((u8*)target, 0x90, target_length);

	iptr address_relative = (iptr)our_function - (iptr)target - 5;

	*(u8*)target = 0xe9;
	*(iptr*)((iptr)target + 1) = address_relative;
	puts("Target");
	util_byte_dump(target, target_length);

	VirtualProtect(target, target_length, protection, &protection);

	return TRUE;
}

internal void* hook_x86_trampoline(void* target, i32 target_length, void* our_function)
{
	printf("[*] Attempting x86 trampoline hook\n");

	puts("Target");
	util_byte_dump(target, target_length);

	//system("pause");

	u8 cleanup[] =
	{
		0x83, 0xc4, 0x10,
		0x5d,
	};

	// NOTE(rhett): Allocate gateway
	u32 gateway_offset = 0;
	u32 gateway_length = target_length + 5 + 5;
	void* gateway = VirtualAlloc(0, gateway_length, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	puts("Gateway");
	util_byte_dump(gateway, gateway_length);

	// TODO(rhett): Create jump to hooked function in gateway?
	*(u8*)((uptr)gateway + gateway_offset) = 0xe8;
	gateway_offset++;
	*(iptr*)((uptr)gateway + gateway_offset) = ((iptr)our_function - (iptr)gateway - 5);
	gateway_offset += sizeof(iptr);
	puts("Gateway");
	util_byte_dump(gateway, gateway_length);

	//TODO(rhett): Cleanup stack for cdecl hook?
	//util_memory_copy((u8*)((uptr)gateway + gateway_offset),
	//cleanup,
	//sizeof(cleanup));
	//
	//gateway_offset += sizeof(cleanup);
	//puts("Gateway");
	//util_byte_dump(gateway, gateway_length);

	// NOTE(rhett): Copy stolen bytes to gateway
	util_memory_copy((u8*)((uptr)gateway + gateway_offset), (u8*)target, target_length);
	gateway_offset += target_length;
	//*(u8*)gateway = 0x55;
	puts("Gateway");
	util_byte_dump(gateway, gateway_length);

	// NOTE(rhett): Create jump back to original code
	iptr gateway_address_relative = ((iptr)target - ((iptr)gateway + gateway_offset));
	*(u8*)((iptr)gateway + gateway_offset) = 0xe9;
	gateway_offset++;
	*(iptr*)((iptr)gateway + gateway_offset) = gateway_address_relative;
	gateway_offset += sizeof(iptr);
	puts("Gateway");
	util_byte_dump(gateway, gateway_length);

	// NOTE(rhett): Create detour to gateway
	hook_x86_detour(target, target_length, gateway);

	//system("pause");

	//system("pause");
	return gateway;
}
#endif

#if defined(X64)
#define JUMP_STUB_LENGTH 14
internal void hook_x64_detour(void* target, usize target_length, void* our_function)
{
	printf("[*] Attempting to detour %p...\n", target);

	u8 jump_stub[JUMP_STUB_LENGTH] =
	{
		0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,            // jmp qword ptr [$+6]
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr
	};
	assert(target_length >= sizeof(jump_stub));

	//puts("Target");
	//util_byte_dump(target, target_length);

	DWORD protection;
	VirtualProtect(target, target_length, PAGE_EXECUTE_READWRITE, &protection);

	util_memory_set((u8*)target, 0x90, target_length);
	util_memory_copy((u8*)((uptr)jump_stub + 6), (u8*)&our_function, sizeof(our_function));
	util_memory_copy((u8*)target, (u8*)jump_stub, sizeof(jump_stub));

	//puts("Target");
	//util_byte_dump(target, target_length);

	VirtualProtect(target, target_length, protection, &protection);
}

internal void* hook_x64_trampoline(void* target, usize target_length, void* our_function)
{
	printf("[*] Attempting to trampoline %p...\n", target);

	puts("Target");
	util_byte_dump(target, target_length);

	isize gateway_offset = 0;
	usize gateway_length = target_length + (JUMP_STUB_LENGTH * 2); // TODO(rhett): ?
	void* gateway = VirtualAlloc(NULL, target_length, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	puts("Gateway");
	util_byte_dump(gateway, gateway_length);

	// NOTE(rhett): Jump to our function
	hook_x64_detour((void*)((uptr)gateway + gateway_offset),
	                JUMP_STUB_LENGTH,
	                our_function);
	gateway_offset += JUMP_STUB_LENGTH;

	puts("Gateway");
	util_byte_dump(gateway, gateway_length);

	// NOTE(rhett): Steal target bytes
	util_memory_copy((u8*)((uptr)gateway + gateway_offset),
	                 (u8*)target,
	                 target_length);
	gateway_offset += target_length;

	puts("Gateway");
	util_byte_dump(gateway, gateway_length);

	// NOTE(rhett): Jump back to after target
	hook_x64_detour((void*)((uptr)gateway + gateway_offset),
	                JUMP_STUB_LENGTH,
	                (void*)((uptr)target + target_length));
	gateway_offset += JUMP_STUB_LENGTH;

	puts("Gateway");
	util_byte_dump(gateway, gateway_length);

	// NOTE(rhett): Install target jump
	hook_x64_detour(target,
	                target_length,
	                gateway);

	puts("Target");
	util_byte_dump(target, target_length);

	//system("pause");
	return NULL;
}

#endif // X64
