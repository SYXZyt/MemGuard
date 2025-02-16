#include "MemGuardWatch.h"
#include <string>
#include <sstream>
#include <MemGuard/MemGuard.h>

void MemGuardWatch::AddAllocation(void* ptr, const char* file, std::size_t line, std::size_t size)
{
	MetaData data;
	data.file = file ? file : "";
	data.line = line;
	data.size = size;

	allocations[ptr] = data;
}

bool MemGuardWatch::RemoveAllocation(void* ptr, const char* file, std::size_t line)
{
	if (!allocations.count(ptr))
	{
		std::stringstream ss;
		ss << "Tried to free a pointer which MemGuard did not allocate";
		if (file)
			ss << "\nAttempted at file: " << file << " at line: " << line;

		memguard_LogMessage(ss.str().c_str());

		return false;
	}

	allocations.erase(ptr);
	return true;
}

void MemGuardWatch::PrintLeaks()
{
	for (auto& pair : allocations)
	{
		std::stringstream ss;
		MetaData& data = pair.second;

		ss << "Memory Leak at 0x" << pair.first << " of size " << data.size;

		if (!data.file.empty())
			ss << "\nCreated at file: " << data.file << " at line: " << data.line;

		memguard_LogMessage(ss.str().c_str());
	}
}