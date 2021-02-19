#ifndef TARGET_FUNCTIONS_H
#define TARGET_FUNCTIONS_H


// TODO: don't hardcode this
uintptr_t baseAddress = 0x140000000;

// Function prototypes
typedef void(__fastcall *LogToFile_t)(uint32_t  unkDword,
                                      char     *fileName,
                                      char     *formatLog,
                                      ...);
LogToFile_t LogToFile = (LogToFile_t)(baseAddress + 0x000315d0);


#endif // TARGET_FUNCTIONS_H