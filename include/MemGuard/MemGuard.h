#pragma once
#include <stddef.h>
#include <stdbool.h>
#include <MemGuard/dllapi.h>

typedef void (*memguard_LogCallback)(const char* message);

#ifdef __cplusplus
#define MEMGUARD_BEGIN extern "C" {
#define MEMGUARD_END }
#else
#define nullptr NULL
#define MEMGUARD_BEGIN
#define MEMGUARD_END
#endif

MEMGUARD_BEGIN

#ifdef MEMGUARD_ENABLE
#define mgMalloc(size) memguard_MallocEx(size, __FILE__, __LINE__)
#define mgCalloc(num, size) memguard_CallocEx(num, size, __FILE__, __LINE__)
#define mgRealloc(ptr, size) memguard_ReallocEx(ptr, size, __FILE__, __LINE__)

#define mgFree(ptr) memguard_FreeEx(ptr, __FILE__, __LINE__)
#else
#define mgMalloc(size) memguard_Malloc(size)
#define mgCalloc(num, size) memguard_Calloc(num, size)
#define mgRealloc(ptr, size) memguard_Realloc(ptr, size)

#define mgFree(ptr) memguard_Free(ptr)
#endif

extern MEMGUARD_API void memguard_Report();

extern MEMGUARD_API void memguard_Init();

extern MEMGUARD_API void* memguard_MallocEx(size_t size, const char* file, int line);
extern MEMGUARD_API void* memguard_CallocEx(size_t num, size_t size, const char* file, int line);
extern MEMGUARD_API void* memguard_ReallocEx(void* ptr, size_t size, const char* file, int line);

extern MEMGUARD_API void memguard_FreeEx(void* ptr, const char* file, int line);

extern MEMGUARD_API void* memguard_Malloc(size_t size);
extern MEMGUARD_API void* memguard_Calloc(size_t num, size_t size);
extern MEMGUARD_API void* memguard_Realloc(void* ptr, size_t size);

extern MEMGUARD_API void memguard_Free(void* ptr);

extern MEMGUARD_API void memguard_SetLogCallback(memguard_LogCallback callback);
extern MEMGUARD_API void memguard_ResetLogCallback();
extern MEMGUARD_API void memguard_LogMessage(const char* message);

extern MEMGUARD_API size_t memguard_GetSize(void* ptr);

MEMGUARD_END