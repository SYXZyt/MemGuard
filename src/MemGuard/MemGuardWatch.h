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
		std::size_t line = 0;
		std::size_t size = 0;
	};

	std::mutex mutex;

	std::unordered_map<void*, MetaData> allocations;

public:
	void AddAllocation(void* ptr, const char* file, std::size_t line, std::size_t size);
	[[nodiscard]] bool RemoveAllocation(void* ptr, const char* file, std::size_t line);
	void PrintLeaks();
	void Reset() { allocations.clear(); }
};