#pragma once
#include <MemGuard/dllapi.h>
#include <stddef.h>

#ifdef __cplusplus
#define	MEMGUARD_BEGIN extern "C" {
#define	MEMGUARD_END }
#else
#define nullptr NULL
#define MEMGUARD_BEGIN
#define MEMGUARD_END
#endif

MEMGUARD_BEGIN

//DO NOT delete this with memguard
typedef struct MemGuardFrame
{
	size_t size;
	struct MemGuardFrame* next;

	const char* file;
	int line;
} MemGuardFrame;

extern MEMGUARD_API MemGuardFrame* memguard_FrameCreate(size_t size, const char* file, int line);
extern MEMGUARD_API void memguard_FrameDestroy(MemGuardFrame* frame);
extern MEMGUARD_API void memguard_FrameDestroyAll(MemGuardFrame* frame);

// Remember to free the string
extern MEMGUARD_API char* memguard_FrameToString(MemGuardFrame* frame);

MEMGUARD_END