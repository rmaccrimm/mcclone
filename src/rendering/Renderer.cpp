#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#include "Renderer.h"
#include "EntityAdmin.h"
#include "ChunkManager.h"
#include "WorldChunk.h"
#include "config.h"

const int VERT_BUFF_SIZE = 500 * (1 << 20);
const int INDEX_BUFF_SIZE = 250 * (1 << 20);

void GLAPIENTRY MessageCallback(
    __attribute__((unused)) GLenum source,
    GLenum type,
    __attribute__((unused)) GLuint id,
    GLenum severity,
    __attribute__((unused)) GLsizei length,
    const GLchar* message,
    __attribute__((unused)) const void* userParam)
{
    fprintf(
        stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
	(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
	type, severity, message
    );
}

int Renderer::initialize()
{
    // Initialize rendering context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_context = SDL_GL_CreateContext(m_window);
    if (m_context == NULL) {
	fprintf(stderr, "Failed to create GL context\n");
	SDL_DestroyWindow(m_window);
	SDL_Quit();
	return 1;
    }

    SDL_GL_SetSwapInterval(1); // Use VSYNC

    // Initialize GL Extension Wrangler (GLEW)
    GLenum err;
    glewExperimental = GL_TRUE; // Please expose OpenGL 3.x+ interfaces
    err = glewInit();
    if (err != GLEW_OK) {
	fprintf(stderr, "Failed to init GLEW\n");
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
	return 1;
    }

    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );
    glEnable(GL_CULL_FACE);
    glEnable (GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    if (initShaders())
	return 1;
    if (initGeometry())
	return 1;

    return 0;
}

/*
 * Initialize Shaders
 */
int Renderer::initShaders()
{
    GLint status;
    char err_buf[512];

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    std::string proj_root = PROJECT_ROOT;
    std::ifstream vert_in(proj_root + "/src/rendering/shaders/solid_color_vert.glsl");
    std::string vert_shader_src;

    vert_in.seekg(0, std::ios::end);   
    vert_shader_src.reserve(vert_in.tellg());
    vert_in.seekg(0, std::ios::beg);

    vert_shader_src.assign(
	(std::istreambuf_iterator<char>(vert_in)),
	std::istreambuf_iterator<char>()
    );

    // Compile vertex shader
    const char* vert_c_str = vert_shader_src.c_str();
    m_vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vert_shader, 1, &vert_c_str, NULL);
    glCompileShader(m_vert_shader);
    glGetShaderiv(m_vert_shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(m_vert_shader, sizeof(err_buf), NULL, err_buf);
        err_buf[sizeof(err_buf)-1] = '\0';
        fprintf(stderr, "Vertex shader compilation failed: %s\n", err_buf);
        return 1;
    }

    std::ifstream frag_in(proj_root + "/src/rendering/shaders/solid_color_frag.glsl");
    std::string frag_shader_src;

    frag_in.seekg(0, std::ios::end);   
    frag_shader_src.reserve(frag_in.tellg());
    frag_in.seekg(0, std::ios::beg);

    frag_shader_src.assign(
	(std::istreambuf_iterator<char>(frag_in)),
	std::istreambuf_iterator<char>()
    );

    // Compile fragment shader
    const char* frag_c_str = frag_shader_src.c_str();
    m_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_frag_shader, 1, &frag_c_str, NULL);
    glCompileShader(m_frag_shader);
    glGetShaderiv(m_frag_shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(m_frag_shader, sizeof(err_buf), NULL, err_buf);
        err_buf[sizeof(err_buf)-1] = '\0';
        fprintf(stderr, "Fragment shader compilation failed: %s\n", err_buf);
        return 1;
    }

    // Link vertex and fragment shaders
    m_shader_prog = glCreateProgram();
    glAttachShader(m_shader_prog, m_vert_shader);
    glAttachShader(m_shader_prog, m_frag_shader);
    glLinkProgram(m_shader_prog);
    glUseProgram(m_shader_prog);

    return 0;
}


/*
 * Initialize Geometry
 */
int Renderer::initGeometry()
{
    // Allocate vertex buffer
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, VERT_BUFF_SIZE, NULL, GL_STREAM_DRAW);

    // Allocate element buffer
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDEX_BUFF_SIZE, NULL, GL_STREAM_DRAW);

    int stride = 9 * sizeof(GLfloat);
    // Bind vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // Bind vertex position attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Bind vertex position attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    return 0;
}


Renderer::Renderer(SDL_Window* window) {
    m_window = window;
    m_vert_buff = std::unique_ptr<GLfloat[]>(new GLfloat[VERT_BUFF_SIZE]);
    m_vert_buff_pos = 0;
    m_index_buff = std::unique_ptr<GLint[]>(new GLint[INDEX_BUFF_SIZE]);
    m_index_buff_pos = 0;
    m_view_matrix = glm::lookAt(glm::vec3(-20.0f, 20.0f, -4.0f),
				glm::vec3(16.0f, 5.0f, 16.0f),
				glm::vec3(0.0f, 1.0f, 0.0));
}

Renderer::~Renderer() {
    glUseProgram(0);
    glDisableVertexAttribArray(0);
    glDetachShader(m_shader_prog, m_vert_shader);
    glDetachShader(m_shader_prog, m_frag_shader);
    glDeleteProgram(m_shader_prog);
    glDeleteShader(m_vert_shader);
    glDeleteShader(m_frag_shader);
    glDeleteTextures(1, &m_tex);
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    // delete[] m_vert_buff;
    // delete[] m_index_buff;
}


void Renderer::clearBuffers()
{
    m_vert_buff_pos = 0;
    m_index_buff_pos = 0;
}

void Renderer::copyVertexData(const std::array<Vertex, 4> &verts,
			      const std::array<int, 6> &inds)
{
    const int vertex_size = 9;
    for (auto it = inds.begin(); it != inds.end(); it++) {
	m_index_buff[m_index_buff_pos++] = *it + m_vert_buff_pos / vertex_size;
    }
    for (auto it = verts.begin(); it != verts.end(); it++) {
	m_vert_buff[m_vert_buff_pos++] = it->position.x;
	m_vert_buff[m_vert_buff_pos++] = it->position.y;
	m_vert_buff[m_vert_buff_pos++] = it->position.z;
	m_vert_buff[m_vert_buff_pos++] = it->color.r;
	m_vert_buff[m_vert_buff_pos++] = it->color.g;
	m_vert_buff[m_vert_buff_pos++] = it->color.b;
	m_vert_buff[m_vert_buff_pos++] = it->normal.x;
	m_vert_buff[m_vert_buff_pos++] = it->normal.y;
	m_vert_buff[m_vert_buff_pos++] = it->normal.z;
    }
}


void Renderer::setViewMatrix(glm::mat4 view_matrix) {
    m_view_matrix = view_matrix;
}

void Renderer::draw() {
    // TODO - don't do this every frame, only for updated chunks
    glBufferSubData(GL_ARRAY_BUFFER,
		    0,
		    m_vert_buff_pos * sizeof(GLfloat),
		    m_vert_buff.get());
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
		    0,
		    m_index_buff_pos * sizeof(GLint),
		    m_index_buff.get());
    
    glUseProgram(m_shader_prog);

    GLint view_loc = glGetUniformLocation(m_shader_prog, "View");
    GLint proj_loc = glGetUniformLocation(m_shader_prog, "Projection");

    auto proj = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);

    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(m_view_matrix));
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, m_index_buff_pos, GL_UNSIGNED_INT, NULL);
    SDL_GL_SwapWindow(m_window);
}
