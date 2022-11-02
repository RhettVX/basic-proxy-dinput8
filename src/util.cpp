//----------------------------------------------------------------
// Function definitions
//----------------------------------------------------------------
internal void util_memory_zero(u8* destination, u32 length)
{
	for (u32 i = 0; i < length; i++)
	{
		destination[i] = 0;
	}
}

internal void util_memory_set(u8* destination, u8 value, u32 length)
{
	for (u32 i = 0; i < length; i++)
	{
		destination[i] = value;
	}
}

internal void util_memory_copy(u8* destination, u8* source, u32 length)
{
	for (u32 i = 0; i < length; i++)
	{
		destination[i] = source[i];
	}
}

internal u32 util_string_length(i8* data)
{
	u32 tail = 0;
	i8 c;
	do
	{
		c = data[tail++];
	}
	while (c != 0);

	return tail;
}

internal void util_string_concat(i8* destination, u32 length, i8* source)
{
	u32 destination_tail;
	for (destination_tail = 0; destination_tail < length;)
	{
		if (!destination[destination_tail])
		{
			break;
		}
		destination_tail++;
	}
	
	i8 c;
	do
	{
		c = source[destination_tail];
		destination[destination_tail++] = c;
	}
	while (c != 0);
}

#if 1
internal void util_log_message(char* message, b32 output_to_console)
{
	if (output_to_console)
	{
		puts(message);
	}

	local_persist FILE* file = fopen("Hook_LogMessages.txt", "w");
	fwrite(message, 1, strlen(message), file);
	fflush(file);
}

// NOTE(rhett): Sloppy but does what I want
internal void util_byte_dump(void* data, u32 length)
{
	u32 bytes_per_line = 8;
	u32 line_count = length / bytes_per_line;
	if (length % bytes_per_line)
	{
		++line_count;
	}

	printf("\n==== Base: %p\n", data);
	for (u32 i = 0; i < line_count; ++i)
	{
		printf("%08X: ", i*bytes_per_line);
		for (u32 j = 0; j < MIN(bytes_per_line, length - (i*bytes_per_line)); ++j)
		{
			if (!(bytes_per_line % 2) && j == bytes_per_line / 2)
			{
				putchar(' ');
			}
			printf("%02x ", *(u8*)((uptr)data + (i*bytes_per_line + j)));
		}
		putchar('\n');
	}
	putchar('\n');

	return;
}
#endif
