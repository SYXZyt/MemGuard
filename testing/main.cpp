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

int main()
{
	MG_PREPARE;
	MyClass* myClass = MemGuard::Allocator::New<MyClass>(5);
	MyClass* myClass2 = MG_ALLOC.New<MyClass>(10);

	MyClass* array = MG_ALLOC.NewArray<MyClass>(5, 1);

	MG_PREPARE;
	MemGuard::Allocator::Delete(myClass);

	printf("Freeing array\n");
	MemGuard::Allocator::DeleteArray(array, 5);

	//Report a leak from myClass2
	MemGuard::Report();
	return 0;
}