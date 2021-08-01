find_package(Catch2 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

include_directories(
	${PROJECT_SOURCE_DIR}/src/
	${PROJECT_SOURCE_DIR}/src/world
	${PROJECT_SOURCE_DIR}/src/entity
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/src/rendering
	${CMAKE_BINARY_DIR}/generated/
)
add_executable(
	tests
	tests.cpp
	test_entities.cpp
	test_collisions.cpp
)
target_link_libraries(
	tests
	PRIVATE
	ecs
	Catch2::Catch2
	renderer
	SDL2::SDL2
	${SDL2TTF_LIBRARY}
	glm
)

