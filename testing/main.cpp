#include <MemGuard/cpp/MemGuard.hpp>
#include <iostream>

int main()
{
	MemGuard::Init(MemGuardFlags::MG_CALLSTACK);

	void* x = malloc(1);
	memguard_FreeEx(x, nullptr, 0);

	MemGuard::Report();
	return 0;
}