set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/)

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} /usr/include/SDL2/)
link_libraries(${GLEW_LIBRARIES})

add_compile_definitions(PROJECT_ROOT="${PROJECT_SOURCE_DIR}")
configure_file(${PROJECT_SOURCE_DIR}/include/config.h.in ${CMAKE_BINARY_DIR}/generated/config.h)

include_directories(
	renderer PRIVATE
	${PROJECT_SOURCE_DIR}/src/
	${PROJECT_SOURCE_DIR}/src/rendering
	${PROJECT_SOURCE_DIR}/src/world
	${PROJECT_SOURCE_DIR}/src/entity
	${PROJECT_SOURCE_DIR}/src/entity/systems
	${CMAKE_BINARY_DIR}/generated/
	${PROJECT_SOURCE_DIR}/include
	SYSTEM INTERFACE  # Suppresses warnings from these files
	${PROJECT_SOURCE_DIR}/include
)

add_library(
	renderer
	rendering/Renderer.cpp
)
target_link_libraries(
	renderer
	GLEW::GLEW
	${OPENGL_gl_LIBRARY}
)
	
add_library(
	ecs
	entity/EntityAdmin.cpp
	InputManager.cpp
	entity/systems/CameraMovementSystem.cpp
	entity/systems/CameraControlSystem.cpp
	entity/systems/PlayerControlSystem.cpp
	entity/systems/MovementSystem.cpp
	entity/systems/PhysicsSystem.cpp
	world/WorldChunk.cpp	
	world/ChunkManager.cpp	
	world/ChunkRenderSystem.cpp
	world/ChunkLoadingSystem.cpp
)

add_executable(
	main
	main.cpp
)
target_link_libraries(
	main
	ecs
	renderer
	SDL2::SDL2
	${SDL2TTF_LIBRARY}
	glm
)
target_compile_options(
	main PRIVATE
	-Wall -Wextra -Wshadow -Wduplicated-cond -Wfatal-errors -Wlogical-op -Wnull-dereference
)
