cmake_minimum_required(VERSION 3.8)
project(MemGuard)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 23)

set(MEMGUARD_SRC src/MemGuard/MemGuard.cpp "src/MemGuard/MemGuardWatch.cpp")

option(MEMGUARD_ENABLE_TRACKING "Enable pointer tracking and leak detection" ON)
option(MEMGUARD_NO_STACKTRACE "Disables the <stacktrace> header for older C++ support" OFF)

add_library(MemGuard SHARED ${MEMGUARD_SRC})
add_library(MemGuardStatic STATIC ${MEMGUARD_SRC})

target_include_directories(MemGuard PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(MemGuardStatic PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

if(MEMGUARD_ENABLE_TRACKING)
	target_compile_definitions(MemGuard PUBLIC MEMGUARD_ENABLE)
	target_compile_definitions(MemGuardStatic PUBLIC MEMGUARD_ENABLE)
endif ()

if (MEMGUARD_NO_STACKTRACE)
	target_compile_definitions(MemGuard PUBLIC MEMGUARD_NO_STACKTRACE)
	target_compile_definitions(MemGuardStatic PUBLIC MEMGUARD_NO_STACKTRACE)
endif ()

add_subdirectory(testing)