#include "MemGuardWatch.h"
#include <string>
#include <sstream>
#include <MemGuard/MemGuard.h>

bool MemGuardWatch::TryRemoveAllocationFromNonStatic(void* ptr)
{
	if (allocations.count(ptr))
	{
		allocations.erase(ptr);
		return true;
	}

	return false;
}

bool MemGuardWatch::TryRemoveAllocationFromStatic(void* ptr)
{
	if (staticAllocations.count(ptr))
	{
		staticAllocations.erase(ptr);
		return true;
	}

	return false;
}

void MemGuardWatch::AddAllocation(void* ptr, const char* file, std::size_t line, std::size_t size, bool isStatic)
{
	std::lock_guard<std::mutex> guard(mutex);

	MetaData data;
	data.file = file ? file : "";
	data.line = line;
	data.size = size;

	(isStatic ? staticAllocations[ptr] : allocations[ptr]) = data;
}

bool MemGuardWatch::RemoveAllocation(void* ptr, const char* file, std::size_t line)
{
	std::lock_guard<std::mutex> guard(mutex);

	if (!TryRemoveAllocationFromNonStatic(ptr) && !TryRemoveAllocationFromStatic(ptr))
	{
		std::stringstream ss;
		ss << "Tried to free a pointer which MemGuard did not allocate";
		if (file)
			ss << "\nAttempted at file: " << file << " at line: " << line;
		memguard_LogMessage(ss.str().c_str());
		return false;
	}

	return true;
}

void MemGuardWatch::PrintLeaks()
{
	std::lock_guard<std::mutex> guard(mutex);

	for (auto& pair : allocations)
	{
		std::stringstream ss;
		MetaData& data = pair.second;

		ss << "Memory Leak at 0x" << pair.first << " of size " << data.size;

		if (!data.file.empty())
			ss << "\nCreated at file: " << data.file << " at line: " << data.line;

		memguard_LogMessage(ss.str().c_str());
	}

	{
		std::stringstream ss;
		ss << "Has " << staticAllocations.size() << " static allocation" << (staticAllocations.size() != 1 ? "s" : "") << " not yet removed";

		memguard_LogMessage(ss.str().c_str());
	}
}