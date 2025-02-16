cmake_minimum_required(VERSION 3.8)
project(MemGuard)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)

set(MEMGUARD_SRC src/MemGuard/MemGuard.cpp "src/MemGuard/MemGuardWatch.cpp")

add_library(MemGuard SHARED ${MEMGUARD_SRC})
add_library(MemGuardStatic STATIC ${MEMGUARD_SRC})

target_include_directories(MemGuard PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(MemGuardStatic PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_compile_definitions(MemGuard PRIVATE MEMGUARD_EXPORT)

add_subdirectory(testing)