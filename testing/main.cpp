#include <MemGuard/cpp/MemGuard.hpp>
#include <iostream>

class MyClass final
{
private:
	int m_value;

public:
	MyClass(int value) : m_value(value) { printf("Constructor %d\n", m_value); }
	~MyClass() { printf("Destructor\n"); }
};

class StaticTestFree final
{
	int* data;

public:
	StaticTestFree()
	{
		data = MG_ALLOC.NewArray<int>(5);
	}

	~StaticTestFree()
	{
		//Since it is an array of ints, we don't need to call DeleteArray since we don't
		//need to call the destructor of each element
		MG_ALLOC.Free(data);
		data = nullptr;
	}
};

class StaticTestNoFree final
{
	int* data;

public:
	StaticTestNoFree()
	{
		data = MG_ALLOC.NewArray<int>(5);
	}
};

static StaticTestFree IFreeData = {};
static StaticTestNoFree INoDontFreeData = {};

int main()
{
	MemGuard::Init();

	IFreeData.~StaticTestFree();

	MemGuard::Report();
	return 0;
}