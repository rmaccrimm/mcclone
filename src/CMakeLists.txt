SET(GCC_DEBUG_COMPILE_FLAGS 
"-fsanitize=address -O0  -pg -ggdb -Wall -Wextra -Wshadow -Wduplicated-cond -Wfatal-errors \
-Wlogical-op -Wnull-dereference --std=c++17"
)
SET(GCC_DEBUG_LINK_FLAGS "-fsanitize=address")

SET(GCC_RELEASE_COMPILE_FLAGS "-O3 -g --std=c++17")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CC_FLAGS_DEBUG} ${GCC_DEBUG_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${GCC_DEBUG_COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${GCC_DEBUG_LINK_FLAGS}")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${GCC_RELEASE_COMPILE_FLAGS}")


find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} include/)
link_libraries(${GLEW_LIBRARIES})

add_compile_definitions(PROJECT_ROOT="${PROJECT_SOURCE_DIR}")
include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(
	main
	main.cpp
	RenderSystem.cpp
	WorldChunk.cpp
	EntityAdmin.cpp
)
target_link_libraries(main
    SDL2::SDL2
    GLEW::GLEW
    ${OPENGL_gl_LIBRARY} 
)
