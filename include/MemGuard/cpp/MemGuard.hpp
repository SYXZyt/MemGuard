#pragma once
#ifndef __cplusplus
#error "Included MemGuard.hpp in a non-C++ file. Only use MemGuard.h when using C"
#endif

#include <MemGuard/MemGuard.h>
#include <new>
#include <utility>

#ifdef MEMGUARD_ENABLE
#define MG_ALLOC MemGuard::Allocator::Prepare(__FILE__, __LINE__)
#define MG_PREPARE do { MemGuard::Allocator::Prepare(__FILE__, __LINE__); } while (false)
#else
#define MG_ALLOC MemGuard::Allocator::Prepare(nullptr, 0)
#define MG_PREPARE do {} while (false)
#endif

namespace MemGuard
{
	inline void Report()
	{
		memguard_Report();
	}

	class Allocator final
	{
	private:
		static Allocator instance;

		static const char* _file;
		static int _line;

		static inline void Reset()
		{
			_file = nullptr;
		}

	public:
		static MEMGUARD_API Allocator& Prepare(const char* file, int line);

		static MEMGUARD_API void* Malloc(size_t size);

		static MEMGUARD_API void* Calloc(size_t num, size_t size);

		static MEMGUARD_API void* Realloc(void* ptr, size_t size);

		static MEMGUARD_API void Free(void* ptr);

		static inline size_t GetSize(void* ptr)
		{
			return memguard_GetSize(ptr);
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
			if (!ptr)
				return;

			ptr->~T();
			Free(ptr);
		}

		template <typename T>
		static inline void DeleteArray(T* ptr)
		{
			if (!ptr)
				return;

			size_t count = GetSize(ptr) / sizeof(T);

			for (size_t i = 0; i < count; i++)
				(
					((T*)ptr) + i
					)->~T();

			Free(ptr);
		}
	};

	inline void Init(MemGuardFlags flags = MemGuardFlags::MG_NONE)
	{
		memguard_Init(flags);
	}

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

	template <typename T, typename... Args>
	inline T* New(Args&&... args)
	{
		return Allocator::New<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	inline T* NewArray(size_t count, Args&&... args)
	{
		return Allocator::NewArray<T>(count, std::forward<Args>(args)...);
	}

	template <typename T>
	static inline void Delete(T* ptr)
	{
		Allocator::Delete<T>(ptr);
	}

	template <typename T>
	static inline void DeleteArray(T* ptr)
	{
		Allocator::DeleteArray<T>(ptr);
	}

	inline void SetLogCallback(memguard_LogCallback callback)
	{
		memguard_SetLogCallback(callback);
	}
}