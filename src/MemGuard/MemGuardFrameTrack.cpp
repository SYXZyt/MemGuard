#include <cstring>
#include <MemGuard/MemGuardFrameTrack.h>
#include <MemGuard/cpp/MemGuard.hpp>
#include <memory>

MemGuardFrame* memguard_FrameCreate(size_t size, const char* file, int line)
{
	MemGuardFrame* frame = new MemGuardFrame();
	std::memset(frame, 0, sizeof(MemGuardFrame));

	frame->size = size;
	frame->file = file;
	frame->line = line;

	frame->next = nullptr;

	return frame;
}

void memguard_FrameDestroy(MemGuardFrame* frame)
{
	if (frame)
		delete frame;
}

void memguard_FrameDestroyAll(MemGuardFrame* frame)
{
	MemGuardFrame* curr = frame;

	while (curr)
	{
		MemGuardFrame* next = curr->next;
		memguard_FrameDestroy(curr);
		curr = next;
	}
}

#include <sstream>
#include <iomanip>

static std::ostream& operator<<(std::ostream& os, MemGuardFrame* frame)
{
	std::stringstream sizeHex;
	sizeHex << std::hex << std::setw(4) << std::setfill('0') << frame->size;

	os << "MemGuardFrame" << "\n";
	os << "Size: " << frame->size << " (0x" << sizeHex.str() << ")\n";
	os << "File: " << frame->file << "\n";
	os << "Line: " << frame->line << "\n";

	return os; 
}

char* memguard_FrameToString(MemGuardFrame* frame)
{
	std::stringstream ss;
	
	MemGuardFrame* curr = frame;
	while (curr)
	{
		ss << curr << '\n';
		curr = curr->next;
	}

	std::string str = ss.str();
	char* cstr = MemGuard::Malloc<char>(str.size() + 1);
	std::strcpy(cstr, str.c_str());

	return cstr;
}