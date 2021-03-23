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
#include "WorldChunk.h"
#include "config.h"

const int VERT_BUFF_SIZE = 500 * (1 << 20);
const int INDEX_BUFF_SIZE = 500 * (1 << 20);

enum CUBE_FACE {
    LEFT = 0,
    RIGHT = 1 ,
    BOTTOM = 2,
    TOP = 3,
    FRONT = 4,
    BACK = 5
};

const glm::vec3 FACE_NORMALS[6]  = {
    // LEFT
    glm::vec3(-1, 0, 0),
    // RIGHT
    glm::vec3(1, 0, 0),
    // BOTTOM
    glm::vec3(0, -1, 0),
    // TOP
    glm::vec3(0, 1, 0),
    // FRONT
    glm::vec3(0, 0, -1),
    // BACK
    glm::vec3(0, 0, 1)
};

// TODO store/use the elements in a constant array as well
const glm::vec3 CUBE_FACES[6][4] = {
    {   // LEFT
	glm::vec3(0, 0, 0),                             
	glm::vec3(0, 0, 1),                             
	glm::vec3(0, 1, 0),                             
	glm::vec3(0, 1, 1),                             
    },
    {   // RIGHT
	glm::vec3(1, 0, 1),
	glm::vec3(1, 0, 0),
	glm::vec3(1, 1, 1),
	glm::vec3(1, 1, 0),
    },
    {   // BOTTOM
	glm::vec3(0, 0, 0),
	glm::vec3(1, 0, 0),
	glm::vec3(0, 0, 1),
	glm::vec3(1, 0, 1),
    },
    {   // TOP
	glm::vec3(1, 1, 0),
	glm::vec3(0, 1, 0),
	glm::vec3(1, 1, 1),
	glm::vec3(0, 1, 1),
    },
    {   // FRONT
	glm::vec3(1, 0, 0),
	glm::vec3(0, 0, 0),
	glm::vec3(1, 1, 0),
	glm::vec3(0, 1, 0),
    },
    {   // BACK
	glm::vec3(0, 0, 1),
	glm::vec3(1, 0, 1),
	glm::vec3(0, 1, 1),
	glm::vec3(1, 1, 1) 
    }
};

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
    glFrontFace(GL_CW);
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    initShaders();
    initGeometry();
    // InitTextures();

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

    // Bind vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    return 0;
}


Renderer::Renderer(SDL_Window* window) {
    m_window = window;
    m_vert_buff.resize(VERT_BUFF_SIZE);
    m_vert_iter = m_vert_buff.begin();
    m_index_buff.resize(INDEX_BUFF_SIZE);
    m_index_iter = m_index_buff.begin();
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

bool checkNeighbour(WorldChunk *chunk, glm::vec3 position, int face) {
    auto n = position + FACE_NORMALS[face];
    return (n.x >= 0 && n.x < WorldChunk::SPAN_X)
	&& (n.y >= 0 && n.y < WorldChunk::SPAN_Y)
	&& (n.z >= 0 && n.z < WorldChunk::SPAN_Z)
	&& chunk->m_blocks[(int)n.x][(int)n.y][(int)n.z];
}

void Renderer::renderChunk(WorldChunk *chunk) {
    m_index_iter = m_index_buff.begin();
    m_vert_iter = m_vert_buff.begin();
    std::vector<int> indices(6, 0);
    std::vector<glm::vec4> face(4);
    
    for (int y = 0; y < WorldChunk::SPAN_Y; y++) {
	for (int x = 0; x < WorldChunk::SPAN_X; x++) {
	    for (int z = 0; z < WorldChunk::SPAN_Z; z++) {
		if (chunk->m_blocks[x][y][z]) {
		    glm::vec3 position = glm::vec3(x, y, z);
		    glm::mat4 translate = glm::translate(glm::mat4(1), position);
		    
		    for (int q = 0; q < 6; q++) {
			if (!checkNeighbour(chunk, position, q)) {
			    // copy indices
			    int i = std::distance(m_vert_buff.begin(), m_vert_iter);
			    indices = {i/3, i/3 + 3, i/3 + 1, i/3, i/3 + 2, i/3 + 3};
			    m_index_iter = std::copy(indices.begin(), indices.end(), m_index_iter);
			    // copy vertices
			    for (int j = 0; j < 4; j++) {
				face[j] = translate * glm::vec4(CUBE_FACES[q][j], 1);
			    }
			    for (auto it = face.begin(); it != face.end(); it++) {
				*m_vert_iter++ = it->x;
				*m_vert_iter++ = it->y;
				*m_vert_iter++ = it->z;
			    }
			}
		    }
                }
            }
        }
    }
    glBufferSubData(GL_ARRAY_BUFFER,
		    0,
		    std::distance(m_vert_buff.begin(), m_vert_iter) * sizeof(GLfloat),
		    m_vert_buff.data());
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
		    0,
		    std::distance(m_index_buff.begin(), m_index_iter) * sizeof(GLint),
		    m_index_buff.data());
}

void Renderer::setViewMatrix(glm::mat4 view_matrix) {
    m_view_matrix = view_matrix;
}

void Renderer::draw() {
    glUseProgram(m_shader_prog);

    GLint view_loc = glGetUniformLocation(m_shader_prog, "View");
    GLint proj_loc = glGetUniformLocation(m_shader_prog, "Projection");

    auto proj = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);

    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(m_view_matrix));
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int n_elements = std::distance(m_index_buff.begin(), m_index_iter);
    glDrawElements(GL_TRIANGLES, n_elements, GL_UNSIGNED_INT, NULL);
    SDL_GL_SwapWindow(m_window);
}
