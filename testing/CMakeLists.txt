cmake_minimum_required(VERSION 3.8)
project(MemGuardTest)

set(CMAKE_CXX_STANDARD 23)

add_executable(MemGuardTest "main.c")
add_executable(MemGuardTestCpp "main.cpp")

target_link_libraries(MemGuardTest MemGuard)
target_link_libraries(MemGuardTestCpp MemGuard)