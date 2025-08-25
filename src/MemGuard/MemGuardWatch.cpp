#include "MemGuardWatch.h"
#include <string>
#include <sstream>
#include <MemGuard/MemGuard.h>

#ifndef MEMGUARD_NO_STACKTRACE
#include <stacktrace>
#endif

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

std::string MemGuardWatch::GetStackTraceFormatted()
{
#ifdef MEMGUARD_NO_STACKTRACE
	return "";
#else
	std::stringstream ss;
	auto trace = std::stacktrace::current();

	int i = 0;
	for (const auto& entry : trace)
	{
		ss << "\t\t";
		ss << trace.size() - 1 - i++ << "| ";

		if (!entry.description().empty())
			ss << entry.description() << "\t\t";

		if (!entry.source_file().empty())
		{
			ss << entry.source_file() << ", ";
			ss << entry.source_line();
		}

		ss << '\n';
	}

	return ss.str();
#endif
}

void MemGuardWatch::AddAllocation(void* ptr, const char* file, std::size_t line, std::size_t size, bool isStatic)
{
	std::lock_guard<std::mutex> guard(mutex);

	MetaData data;
	data.file = file ? file : "";
	data.line = line;
	data.size = size;

	data.stackTrace = stackTrace ? GetStackTraceFormatted() : "";

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

#ifndef MEMGUARD_NO_STACKTRACE
		if (stackTrace)
		{
			auto trace = std::stacktrace::current();
			if (trace.size() > 1)
			{
				ss << "\n\tStack Trace:\n";
				ss << GetStackTraceFormatted();
				ss << '\n';
			}

			memguard_LogMessage(ss.str().c_str());
			return false;
		}
#endif
		memguard_LogMessage(ss.str().c_str());
	}

	return true;
}

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void MemGuardWatch::PrintLeaks()
{
	std::lock_guard<std::mutex> guard(mutex);

	char userProfile[MAX_PATH];
	std::string userPath;
	if (ExpandEnvironmentStringsA("%USERPROFILE%", userProfile, MAX_PATH))
		userPath = userProfile;

	for (auto& pair : allocations)
	{
		std::stringstream ss;
		MetaData& data = pair.second;

		ss << "Memory Leak at 0x" << pair.first << " of size " << data.size;

		if (!data.file.empty())
		{
			ss << "\nCreated at file: ";

			std::string fileName = data.file;

			if (!userPath.empty() && fileName.compare(0, userPath.size(), userPath) == 0)
				fileName = fileName.substr(userPath.size());

			ss << fileName << " at line: " << data.line;
		}

		if (stackTrace && !data.stackTrace.empty())
		{
			std::string stack = data.stackTrace;

			if (!userPath.empty())
			{
				// Replace all occurrences of userPath in stackTrace with empty string
				size_t pos = 0;

				while ((pos = stack.find(userPath, pos)) != std::string::npos)
				{
					stack.erase(pos, userPath.size());
				}
			}

			ss << "\n\tStack Trace:\n" << stack << '\n';
		}

		memguard_LogMessage(ss.str().c_str());
	}

	if (!staticAllocations.empty())
	{
		std::stringstream ss;
		ss << "Has " << staticAllocations.size() << " static allocation" << (staticAllocations.size() != 1 ? "s" : "")
			<< " not yet removed";

		memguard_LogMessage(ss.str().c_str());
	}
}