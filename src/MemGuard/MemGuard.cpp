#include <MemGuard/MemGuard.h>
#include <MemGuard/cpp/MemGuard.hpp>
#include "MemGuardWatch.h"

#include <cstring>

struct IsStaticTime final
{
	bool isStaticTime = true;
} static staticTrack;

MemGuard::Allocator MemGuard::Allocator::instance = {};
const char* MemGuard::Allocator::_file = nullptr;
int MemGuard::Allocator::_line = 0;

using u8 = unsigned char;

static MemGuardWatch watcher;

static constexpr bool monitorPointers =
#ifdef MEMGUARD_ENABLE
true;
#else
false;
#endif

MemGuard::Allocator& MemGuard::Allocator::Prepare(const char* file, int line)
{
	Allocator::_file = file;
	Allocator::_line = line;

	return instance;
}

void* MemGuard::Allocator::Malloc(size_t size)
{
	void* ptr = memguard_MallocEx(size, _file, _line);
	Reset();

	return ptr;
}

void* MemGuard::Allocator::Calloc(size_t num, size_t size)
{
	void* ptr = memguard_CallocEx(num, size, _file, _line);
	Reset();

	return ptr;
}

void* MemGuard::Allocator::Realloc(void* ptr, size_t size)
{
	void* newPtr = memguard_ReallocEx(ptr, size, _file, _line);
	Reset();

	return newPtr;
}

void MemGuard::Allocator::Free(void* ptr)
{
	if (!ptr)
		return;

	memguard_FreeEx(ptr, _file, _line);
	Reset();
}

static bool lock = false;

static void DefaultLogCallback(const char* message)
{
	printf("[MemGuard] %s\n", message);
}

static memguard_LogCallback logCallback = DefaultLogCallback;

void memguard_Report()
{
	if (!monitorPointers)
		return;

	watcher.PrintLeaks();
	watcher.Reset();
}

void memguard_Init(MemGuardFlags flags)
{
	staticTrack.isStaticTime = false;

	watcher.StackTrace(flags & MG_CALLSTACK);
}

void* memguard_MallocEx(size_t size, const char* file, int line)
{
	lock = true;
	void* ptr = memguard_Malloc(size);
	lock = false;

	watcher.AddAllocation(ptr, file, line, size, staticTrack.isStaticTime);

	return ptr;
}

void* memguard_CallocEx(size_t num, size_t size, const char* file, int line)
{
	lock = true;
	void* ptr = memguard_Calloc(num, size);
	lock = false;

	watcher.AddAllocation(ptr, file, line, num * size, staticTrack.isStaticTime);

	return ptr;
}

void* memguard_ReallocEx(void* ptr, size_t size, const char* file, int line)
{
	lock = true;
	void* newPtr = memguard_Realloc(ptr, size);
	lock = false;

	bool _ = watcher.RemoveAllocation(ptr, file, line);
	watcher.AddAllocation(newPtr, file, line, size, staticTrack.isStaticTime);

	return newPtr;
}

void memguard_FreeEx(void* ptr, const char* file, int line)
{
	if (!ptr)
		return;

	if (!watcher.RemoveAllocation(ptr, file, line))
		return;

	lock = true;
	memguard_Free(ptr);
	lock = false;
}

void* memguard_Malloc(size_t size)
{
	void* ptr = new u8[size];

	if (!lock)
		watcher.AddAllocation(ptr, nullptr, 0, size, staticTrack.isStaticTime);

	return ptr;
}

void* memguard_Calloc(size_t num, size_t size)
{
	void* ptr = new u8[num * size];
	memset(ptr, 0, num * size);

	if (!lock)
		watcher.AddAllocation(ptr, nullptr, 0, num * size, staticTrack.isStaticTime);

	return ptr;
}

void* memguard_Realloc(void* ptr, size_t size)
{
	if (!ptr)
		return new u8[size];

	if (size == 0)
	{
		delete[](u8*)ptr;
		return nullptr;
	}

	u8* newPtr = new u8[size];

	size_t oldSize = watcher.GetSize(ptr);
	memcpy(newPtr, ptr, std::min(oldSize, size));

	if (!lock)
	{
		bool _ = watcher.RemoveAllocation(ptr, nullptr, 0);
		watcher.AddAllocation(newPtr, nullptr, 0, size, staticTrack.isStaticTime);
	}

	delete[](u8*)ptr;

	return newPtr;
}


void memguard_Free(void* ptr)
{
	if (!ptr)
		return;

	if (!lock)
		if (!watcher.RemoveAllocation(ptr, nullptr, 0))
			return;

	delete[](u8*)ptr;
}

void memguard_SetLogCallback(memguard_LogCallback callback)
{
	logCallback = callback;
}

void memguard_ResetLogCallback()
{
	logCallback = DefaultLogCallback;
}

void memguard_LogMessage(const char* message)
{
	logCallback(message);
}

size_t memguard_GetSize(void* ptr)
{
	return watcher.GetSize(ptr);
}

bool memguard_IsOwned(void* ptr)
{
	return watcher.IsOwned(ptr);
}