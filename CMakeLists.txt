cmake_minimum_required(VERSION 3.9)
project(mcclone)

SET(
	GCC_DEBUG_COMPILE_FLAGS 
	"-fsanitize=address -O0  -pg -ggdb -Wall -Wextra -Wshadow -Wduplicated-cond -Wfatal-errors \
	-Wlogical-op -Wnull-dereference --std=c++20 -I/usr/include/SDL2 -D_REENTRANT"
)

SET(GCC_DEBUG_LINK_FLAGS "-fsanitize=address -pthread")
SET(GCC_RELEASE_COMPILE_FLAGS "-O3 -g --std=c++20")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CC_FLAGS_DEBUG} ${GCC_DEBUG_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${GCC_DEBUG_COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${GCC_DEBUG_LINK_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${GCC_RELEASE_COMPILE_FLAGS}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(src)
add_subdirectory(test)
