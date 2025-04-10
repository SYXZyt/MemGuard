#pragma once
#include <unordered_map>
#include <string>
#include <mutex>

class MemGuardWatch final
{
private:
	struct MetaData final
	{
		std::string file;
		std::string stackTrace;
		std::size_t line = 0;
		std::size_t size = 0;
	};

	bool stackTrace = false;

	std::mutex mutex;
	std::unordered_map<void*, MetaData> allocations;
	std::unordered_map<void*, MetaData> staticAllocations;

	[[nodiscard]]
	bool TryRemoveAllocationFromNonStatic(void* ptr);

	[[nodiscard]]
	bool TryRemoveAllocationFromStatic(void* ptr);

	std::string GetStackTraceFormatted();

public:
	void StackTrace(bool enable) { stackTrace = enable; }

	inline size_t GetSize(void* ptr)
	{
		std::lock_guard<std::mutex> guard(mutex);

		if (allocations.count(ptr))
			return allocations.at(ptr).size;

		if (staticAllocations.count(ptr))
			return staticAllocations.at(ptr).size;

		return 0;
	}

	inline bool IsOwned(void* ptr)
	{
		std::lock_guard<std::mutex> guard(mutex);

		if (allocations.count(ptr))
			return true;

		if (staticAllocations.count(ptr))
			return true;

		return false;
	}

	void AddAllocation(void* ptr, const char* file, std::size_t line, std::size_t size, bool isStatic);
	[[nodiscard]] bool RemoveAllocation(void* ptr, const char* file, std::size_t line);
	void PrintLeaks();
	void Reset() { allocations.clear(); }
};