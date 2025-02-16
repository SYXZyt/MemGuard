#pragma once
#ifndef __cplusplus
#error "Included MemGuard.hpp in a non-C++ file. Only use MemGuard.h when using C"
#endif

#include <MemGuard/MemGuard.h>
#include <new>
#include <utility>

#define MG_ALLOC MemGuard::Allocator::Prepare(__FILE__, __LINE__)
#define MG_PREPARE MemGuard::Allocator::Prepare(__FILE__, __LINE__)

namespace MemGuard
{
	inline void Init(bool monitorPointers = true)
	{
		memguard_Init(monitorPointers);
	}

	inline void Shutdown()
	{
		memguard_Shutdown();
	}

	class Allocator final
	{
	private:
		static inline const char* _file = nullptr;
		static inline int _line = 0;

	public:
		static Allocator& Prepare(const char* file, int line)
		{
			static Allocator instance;
			instance._file = file;
			instance._line = line;

			return instance;
		}

		static inline void* Malloc(size_t size)
		{
			return memguard_MallocEx(size, _file, _line);
		}

		static inline void* Calloc(size_t num, size_t size)
		{
			return memguard_CallocEx(num, size, _file, _line);
		}

		static inline void* Realloc(void* ptr, size_t size)
		{
			return memguard_ReallocEx(ptr, size, _file, _line);
		}

		static inline void Free(void* ptr)
		{
			memguard_FreeEx(ptr, _file, _line);
		}

		template <typename T, typename... Args>
		static inline T* New(Args&&... args)
		{
			void* ptr = Malloc(sizeof(T));

			return new (ptr) T(std::forward<Args>(args)...);
		}

		template <typename T, typename... Args>
		static inline T* NewArray(size_t count, Args&&... args)
		{
			void* ptr = Malloc(sizeof(T) * count);
			for (size_t i = 0; i < count; i++)
				new (((T*)ptr) + i) T(std::forward<Args>(args)...);

			return (T*)ptr;
		}

		template <typename T>
		static inline void Delete(T* ptr)
		{
			ptr->~T();
			Free(ptr);
		}

		template <typename T>
		static inline void DeleteArray(T* ptr, size_t count)
		{
			for (size_t i = 0; i < count; i++)
				(
					((T*)ptr) + i
					)->~T();

			Free(ptr);
		}
	};

	inline void* Malloc(size_t size)
	{
		return Allocator::Malloc(size);
	}

	inline void* Calloc(size_t num, size_t size)
	{
		return Allocator::Calloc(num, size);
	}

	inline void* Realloc(void* ptr, size_t size)
	{
		return Allocator::Realloc(ptr, size);
	}

	inline void Free(void* ptr)
	{
		Allocator::Free(ptr);
	}

	inline void SetLogCallback(memguard_LogCallback callback)
	{
		memguard_SetLogCallback(callback);
	}
}