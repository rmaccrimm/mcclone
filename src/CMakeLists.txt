find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})

add_compile_definitions(PROJECT_ROOT="${PROJECT_SOURCE_DIR}")
configure_file(${PROJECT_SOURCE_DIR}/src/include/config.h.in ${CMAKE_BINARY_DIR}/generated/config.h)

include_directories(
	${PROJECT_SOURCE_DIR}/src/
	${PROJECT_SOURCE_DIR}/src/include
	${PROJECT_SOURCE_DIR}/src/rendering
	${CMAKE_BINARY_DIR}/generated/
)

add_library(
	lib
	rendering/Renderer.cpp
	WorldChunk.cpp
	ChunkManager.cpp
	EntityAdmin.cpp
	InputManager.cpp
	CameraMovementSystem.cpp
)
add_executable(
	main
	main.cpp
)
target_link_libraries(
	main
	lib
	SDL2::SDL2
	GLEW::GLEW
	${OPENGL_gl_LIBRARY} 
)
