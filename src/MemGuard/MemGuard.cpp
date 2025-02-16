#include <MemGuard/MemGuard.h>
#include "MemGuardWatch.h"

static MemGuardWatch watcher;

static constexpr bool monitorPointers = 
#ifdef MEMGUARD_ENABLE
true;
#else
false;
#endif

static bool lock = false;

static void DefaultLogCallback(const char* message)
{
	printf("[MemGuard] %s\n", message);
}

static memguard_LogCallback logCallback = DefaultLogCallback;

void memguard_Report()
{
	watcher.PrintLeaks();
	watcher.Reset();
}

void* memguard_MallocEx(size_t size, const char* file, int line)
{
	lock = true;
	void* ptr = memguard_Malloc(size);
	lock = false;

	if (monitorPointers)
		watcher.AddAllocation(ptr, file, line, size);

	return ptr;
}

void* memguard_CallocEx(size_t num, size_t size, const char* file, int line)
{
	lock = true;
	void* ptr = memguard_Calloc(num, size);
	lock = false;

	if (monitorPointers)
		watcher.AddAllocation(ptr, file, line, num * size);

	return ptr;
}

void* memguard_ReallocEx(void* ptr, size_t size, const char* file, int line)
{
	lock = true;
	void* newPtr = memguard_Realloc(ptr, size);
	lock = false;

	if (monitorPointers)
	{
		bool _ = watcher.RemoveAllocation(ptr, file, line);
		watcher.AddAllocation(newPtr, file, line, size);
	}

	return newPtr;
}

void memguard_FreeEx(void* ptr, const char* file, int line)
{
	if (!ptr)
		return;

	if (monitorPointers)
	{
		if (!watcher.RemoveAllocation(ptr, file, line))
			return;
	}

	lock = true;
	memguard_Free(ptr);
	lock = false;
}

void* memguard_Malloc(size_t size)
{
	void* ptr = std::malloc(size);

	if (monitorPointers && !lock)
		watcher.AddAllocation(ptr, nullptr, 0, size);

	return ptr;
}

void* memguard_Calloc(size_t num, size_t size)
{
	void* ptr = std::calloc(num, size);

	if (monitorPointers && !lock)
		watcher.AddAllocation(ptr, nullptr, 0, num * size);

	return ptr;
}

void* memguard_Realloc(void* ptr, size_t size)
{
	void* newPtr = std::realloc(ptr, size);

	if (monitorPointers && !lock)
	{
		bool _ = watcher.RemoveAllocation(ptr, nullptr, 0);
		watcher.AddAllocation(newPtr, nullptr, 0, size);
	}

	return newPtr;
}

void memguard_Free(void* ptr)
{
	if (!ptr)
		return;

	if (monitorPointers && !lock)
		if (!watcher.RemoveAllocation(ptr, nullptr, 0))
			return;

	std::free(ptr);
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