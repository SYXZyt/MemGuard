#pragma once
#include <stddef.h>
#include <stdbool.h>
#include <MemGuard/dllapi.h>
#include <MemGuard/MemGuardFrameTrack.h>

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

#define MG_FLAG(id) = (1 << id)

typedef enum MemGuardFlags
{
	MG_NONE 		= 0,
	MG_CALLSTACK	MG_FLAG(0),
} MemGuardFlags;

#ifdef MEMGUARD_ENABLE
#define memguard_Malloc(size) memguard_MallocEx(size, __FILE__, __LINE__)
#define memguard_Calloc(num, size) memguard_CallocEx(num, size, __FILE__, __LINE__)
#define memguard_Realloc(ptr, size) memguard_ReallocEx(ptr, size, __FILE__, __LINE__)

#define memguard_Free(ptr) memguard_FreeEx(ptr, __FILE__, __LINE__)
#else
#define memguard_Malloc(size) memguard_MallocReal(size)
#define memguard_Calloc(num, size) memguard_CallocReal(num, size)
#define memguard_Realloc(ptr, size) memguard_ReallocReal(ptr, size)

#define memguard_Free(ptr) memguard_Free(ptr)
#endif

extern MEMGUARD_API void memguard_Report();

extern MEMGUARD_API void memguard_Init(MemGuardFlags flags);

extern MEMGUARD_API void memguard_BeginFrame();
extern MEMGUARD_API MemGuardFrame* memguard_EndFrame();

extern MEMGUARD_API void* memguard_MallocEx(size_t size, const char* file, int line);
extern MEMGUARD_API void* memguard_CallocEx(size_t num, size_t size, const char* file, int line);
extern MEMGUARD_API void* memguard_ReallocEx(void* ptr, size_t size, const char* file, int line);

extern MEMGUARD_API void memguard_FreeEx(void* ptr, const char* file, int line);

extern MEMGUARD_API void* memguard_MallocReal(size_t size);
extern MEMGUARD_API void* memguard_CallocReal(size_t num, size_t size);
extern MEMGUARD_API void* memguard_ReallocReal(void* ptr, size_t size);

extern MEMGUARD_API void memguard_FreeReal(void* ptr);

extern MEMGUARD_API void memguard_SetLogCallback(memguard_LogCallback callback);
extern MEMGUARD_API void memguard_ResetLogCallback();
extern MEMGUARD_API void memguard_LogMessage(const char* message);

extern MEMGUARD_API size_t memguard_GetSize(void* ptr);

extern MEMGUARD_API bool memguard_IsOwned(void* ptr);

MEMGUARD_END