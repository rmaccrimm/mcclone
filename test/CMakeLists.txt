find_package(Catch2 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(
	${PROJECT_SOURCE_DIR}/src/
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/src/rendering
	${CMAKE_BINARY_DIR}/generated/
)
add_executable(
	tests
	tests.cpp
	test_entities.cpp)
target_link_libraries(
	tests
	PRIVATE
	lib
	Catch2::Catch2
	SDL2::SDL2
	GLEW::GLEW
	${OPENGL_gl_LIBRARY} 
)

