#pragma once
#ifdef _WIN32
	#ifdef MEMGUARD_EXPORT
		#define MEMGUARD_API __declspec(dllexport)
	#else
		#define MEMGUARD_API __declspec(dllimport)
	#endif
#else
	#define MEMGUARD_API
#endif