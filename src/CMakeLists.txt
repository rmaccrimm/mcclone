set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/)

find_package(SDL2 REQUIRED)
find_package(SDL2TTF REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} /usr/include/SDL2/)
link_libraries(${GLEW_LIBRARIES})

add_compile_definitions(PROJECT_ROOT="${PROJECT_SOURCE_DIR}")
configure_file(${PROJECT_SOURCE_DIR}/include/config.h.in ${CMAKE_BINARY_DIR}/generated/config.h)

add_library(
	lib
	rendering/Renderer.cpp
	rendering/RenderSystem.cpp
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
	${SDL2TTF_LIBRARY}
	GLEW::GLEW
	${OPENGL_gl_LIBRARY}
	glm
)
target_compile_options(
	main
	PRIVATE -Wall -Wextra -Wshadow -Wduplicated-cond -Wfatal-errors -Wlogical-op
	-Wnull-dereference
)
target_include_directories(lib PRIVATE
	${PROJECT_SOURCE_DIR}/src/
	${PROJECT_SOURCE_DIR}/src/rendering
	${CMAKE_BINARY_DIR}/generated/
	${PROJECT_SOURCE_DIR}/include
	SYSTEM INTERFACE
	${PROJECT_SOURCE_DIR}/include
)

target_include_directories(main PRIVATE
	${PROJECT_SOURCE_DIR}/src/
	${PROJECT_SOURCE_DIR}/src/rendering
	${CMAKE_BINARY_DIR}/generated/
	${PROJECT_SOURCE_DIR}/include
	SYSTEM INTERFACE
	${PROJECT_SOURCE_DIR}/include
)

