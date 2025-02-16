# MemGuard
MemGuard is a library for helping detect and prevent memory leaks in C and C++ programs.
MemGuard provides its own memory allocation functions that can be used in place of malloc, calloc, realloc, and free, as well as handling constructors and destructors for C++ objects.
MemGuard will report any memory that is not freed when the program exits.

## Usage
### Static or Shared Library
MemGuard can be built into both a static or shared library. Generally shared is preferred since it allows all libraries to use the same memory management functions.
Say you have a game engine, if your engine is a shared library, use MemGuard as a shared library. If your engine is a static library, use MemGuard as a static library.

### Macros
To enable the tracking system, you will need to set `MEMGUARD_ENABLE_TRACKING` to `ON` in the CMake configuration, or before including the sub directory. (It is set to `ON` by default)
This will define the `MEMGUARD_ENABLE` macros for both the library, and your project. If you do not use CMake, make sure to define that macro when building the library.

You should only define this for debug builds of your application. The system can use a lot of memory and slow down the program so it is not recommended for release builds.

MemGuard provides macros to provide additional data to the watch system. This is done so that when MemGuard reports leaks, it will tell you where the object was created.
This isn't perfect since if you have a string class, it will tell you that the string caused the leak, but not where that string was made. In the future, a callstack log may also be added.
To use the extra data, you must define the following macro: `MEMGUARD_FULL`

The macros defined are the following:
| Macro                   | Overridden function                          |
| ----------------------- | -------------------------------------------- |
| `mgMalloc(size)`        | `memguard_MallocEx(size, file, line)`        |
| `mgCalloc(count, size)` | `memguard_CallocEx(count, size, file, line)` |
| `mgRealloc(ptr, size)`  | `memguard_ReallocEx(ptr, size, file, line)`  |
| `mgFree(ptr)`           | `memguard_FreeEx(ptr, file, line)`           |

If you are using C++, then two macros will be defined for you. These override the same function and simply prepare the file and line for the allocated.
If you use the macros, then you gain the benefit of the extra data, but it does make the code look worse in my opinion.

### C
For C, memguard is as simple as using normal memory functions.
Just make sure that you call `memguard_Init(enablePointerTracking)` at the top of main, and `memguard_Report()` at the end of main.
The shutdown function will report any memory that was not freed and will clear all pointers, ready to be used again.

```c
typedef struct Vec2
{
	float x;
	float y;
} Vec2;

int main(int argc, char** argv)
{
	Vec2* vec = mgMalloc(sizeof(Vec2));
	Vec2* vec2 = mgMalloc(sizeof(Vec2));

	int* array = mgCalloc(10, sizeof(int));

	mgFree(vec);
	mgFree(array);

	//Reports one leak from vec2
	memguard_Report();

	return 0;
}
```

Output:
```
[MemGuard] Memory Leak at 0x00000280BDFC79D0 of size 8
Created at file: C:\Users\xxjak\source\repos\MemGuard\testing\main.c at line: 15
```

### C++
While you can use the C functions in C++, MemGuard provides a `.hpp` header you can use.
This contains a namespace and class to redirect calls the to C functions.
This header also contains a `New` and `Delete` so you can use it on objects to call the constructor and destructor.

Another macro can be used to set the extra data. This is either `MEMGUARD_ALLOC` or `MEMGUARD_PREPARE`. These macros return a reference to the allocator so you can directly call a function after the macro.

Like with C, make sure to shutdown the library with the report function.

```cpp
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
```

Output:
```
Constructor 5
Constructor 10
Constructor 1
Constructor 1
Constructor 1
Constructor 1
Constructor 1
Destructor
Freeing array
Destructor
Destructor
Destructor
Destructor
Destructor
[MemGuard] Memory Leak at 0x000001A19CB689B0 of size 4
Created at file: C:\Users\xxjak\source\repos\MemGuard\testing\main.cpp at line: 20
```

You can also prepare the allocator yourself.
```cpp
MemGuard::Allocator::Prepare(__FILE__, __LINE__);
```

### Callback
You can provide a function to be called which will pass you a string of the leak information. You can then do whatever you want with this information.
This is done by calling `memguard_SetLogCallback` and passing a function pointer to a function that takes a `const char*` as a parameter.
Additionally using C++, you can use a lambda function to capture variables.
```cpp
#include <MemGuard/cpp/MemGuard.hpp>

int main()
{
	MemGuard::Init();
	MG_PREPARE;
	int* array = MG_ALLOC.NewArray<int>(5, 1);
	MemGuard::SetLogCallback([](const char* msg) { printf("Leak: %s\n", msg); });
	MemGuard::Shutdown();
	return 0;
}
```

## Known Issues
- The library is not thread safe. If you are using threads, you must ensure that only one thread is using the library at a time.
- If using the C++ header, then you cannot allocate an array of pointers using the C++ `NewArray` allocator. You must manually do it with the other functions.

## Building
This library uses CMake. To build, it is as simple as including the source and running `add_subdirectory`.
Then link either the shared or static library to your project. These use the target names `MemGuard` and `MemGuardStatic` respectively.
Finally, add the include folder which is `MemGuardDir/include/`.

## Future Plans
- Add a callstack log to show where the object was created.
- Add thread safety.