#include <CImg.h>
// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
// clang-format on
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/mat4x4.hpp>
#include <plog/Log.h>

#include <fstream>
#include <streambuf>
#include <string>
#include <vector>

#include "ChunkManager.h"
#include "EntityAdmin.h"
#include "Renderer.h"
#include "WorldChunk.h"
#include "config.h"

const int VERT_BUFF_SIZE = 500 * (1 << 20);
const int INDEX_BUFF_SIZE = 250 * (1 << 20);

// clang-format off
const float SCREEN_QUAD[] = {
//   position    texture coords
    -1,  1, 0,   0, 1,
    -1, -1, 0,   0, 0,
     1, -1, 0,   1, 0,
    -1,  1, 0,   0, 1,
     1, -1, 0,   1, 0,
     1,  1, 0,   1, 1
};
// clang-format on

void GLAPIENTRY MessageCallback(
    __attribute__((unused)) GLenum source,
    __attribute__((unused)) GLenum type,
    __attribute__((unused)) GLuint id,
    __attribute__((unused)) GLenum severity,
    __attribute__((unused)) GLsizei length,
    const GLchar* message,
    __attribute__((unused)) const void* userParam)
{
    if (std::string(message) != "") {
        LOG_INFO << "* GL Message * " << message;
    }
}

Renderer::Renderer(SDL_Window* window)
{
    m_window = window;
    m_vert_buff = std::unique_ptr<GLfloat[]>(new GLfloat[VERT_BUFF_SIZE]);
    m_vert_buff_pos = 0;
    m_index_buff = std::unique_ptr<GLint[]>(new GLint[INDEX_BUFF_SIZE]);
    m_index_buff_pos = 0;
    m_view_matrix = glm::lookAt(
        glm::vec3(-20.0f, 20.0f, -4.0f), glm::vec3(16.0f, 5.0f, 16.0f), glm::vec3(0.0f, 1.0f, 0.0));
}

Renderer::~Renderer()
{
    glUseProgram(0);
    glDisableVertexAttribArray(0);

    for (auto& p : m_shader_prog_map) {
        glDeleteProgram(p.second);
    }

    glDeleteTextures(1, &m_scene.tex);
    glDeleteBuffers(1, &m_scene.ebo);
    glDeleteBuffers(1, &m_scene.vbo);
    glDeleteVertexArrays(1, &m_scene.vao);
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    // delete[] m_vert_buff;
    // delete[] m_index_buff;
}

int Renderer::initialize()
{
    LOG_INFO << "Initializing renderer";

    // Initialize rendering context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_context = SDL_GL_CreateContext(m_window);
    if (m_context == NULL) {
        LOG_ERROR << "Failed to create GL context";
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 1;
    }

    SDL_GL_SetSwapInterval(1); // Use VSYNC

    // Initialize
    LOG_INFO << "Initializing GL Extension Wrangler (GLEW)";
    GLenum err;
    glewExperimental = GL_TRUE; // Please expose OpenGL 3.x+ interfaces
    err = glewInit();
    if (err != GLEW_OK) {
        LOG_ERROR << "Failed to init GLEW";
        SDL_GL_DeleteContext(m_context);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 1;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    LOG_INFO << "Generating VAOs";
    glGenVertexArrays(1, &m_scene.vao);
    glBindVertexArray(m_scene.vao);

    if (initShader("solid_color") || initShader("screen"))
        return 1;
    if (initGeometry())
        return 1;
    if (initTextures())
        return 1;

    return 0;
}

int loadShader(std::string fname, GLuint& shader_id, GLuint shader_type)
{
    LOG_INFO << "Compiling shader " << fname << "...";
    GLint status;
    char err_buf[512];

    // Load shader source
    std::string proj_root = PROJECT_ROOT;
    std::ifstream shader_in(proj_root + fname);
    std::string shader_src;

    shader_in.seekg(0, std::ios::end);
    shader_src.reserve(shader_in.tellg());
    shader_in.seekg(0, std::ios::beg);

    shader_src.assign(
        (std::istreambuf_iterator<char>(shader_in)), std::istreambuf_iterator<char>());

    // Compile shader
    const char* shader_c_str = shader_src.c_str();

    shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &shader_c_str, NULL);
    glCompileShader(shader_id);
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(shader_id, sizeof(err_buf), NULL, err_buf);
        err_buf[sizeof(err_buf) - 1] = '\0';
        LOG_ERROR << "Vertex shader compilation failed:\n" << err_buf;
        return 1;
    }
    return 0;
}

/*
 * Initialize Shaders
 */
int Renderer::initShader(std::string shader_name)
{
    GLuint vert_shader;
    GLuint frag_shader;

    if (loadShader(
            "/src/rendering/shaders/" + shader_name + "_vert.glsl",
            vert_shader,
            GL_VERTEX_SHADER)) {
        return 1;
    }

    if (loadShader(
            "/src/rendering/shaders/" + shader_name + "_frag.glsl",
            frag_shader,
            GL_FRAGMENT_SHADER)) {
        return 1;
    }

    // Link vertex and fragment shaders
    m_shader_prog_map[shader_name] = glCreateProgram();
    auto shader_prog = m_shader_prog_map[shader_name];

    glAttachShader(shader_prog, vert_shader);
    glAttachShader(shader_prog, frag_shader);
    glLinkProgram(shader_prog);

    glDetachShader(shader_prog, vert_shader);
    glDetachShader(shader_prog, frag_shader);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    GLint status;
    char err_buf[512];

    glGetProgramiv(shader_prog, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(shader_prog, sizeof(err_buf), NULL, err_buf);
        err_buf[sizeof(err_buf) - 1] = '\0';
        fprintf(stderr, "Vertex shader compilation failed: %s\n", err_buf);
        return 1;
    }

    glUseProgram(shader_prog);

    return 0;
}

/*
 * Initialize Geometry
 */
int Renderer::initGeometry()
{
    // Allocate vertex buffer
    glGenBuffers(1, &m_scene.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_scene.vbo);
    glBufferData(GL_ARRAY_BUFFER, VERT_BUFF_SIZE, NULL, GL_STREAM_DRAW);

    // Allocate element buffer
    glGenBuffers(1, &m_scene.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_scene.ebo);
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

int Renderer::initTextures()
{
    glGenTextures(1, &m_frame_buffer);
    glBindTexture(GL_TEXTURE_2D, m_frame_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenRenderbuffers(1, &m_depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_buffer);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_frame_buffer, 0);

    GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, draw_buffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return 1;
    }

    return 0;
}

void Renderer::clearBuffers()
{
    m_vert_buff_pos = 0;
    m_index_buff_pos = 0;
}

void Renderer::copyVertexData(const std::array<Vertex, 4>& verts, const std::array<int, 6>& inds)
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

void Renderer::setViewMatrix(glm::mat4 view_matrix) { m_view_matrix = view_matrix; }

void Renderer::draw()
{
    // TODO - don't do this every frame, only for updated chunks
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vert_buff_pos * sizeof(GLfloat), m_vert_buff.get());
    glBufferSubData(
        GL_ELEMENT_ARRAY_BUFFER, 0, m_index_buff_pos * sizeof(GLint), m_index_buff.get());

    auto shader_prog = m_shader_prog_map["solid_color"];
    glUseProgram(shader_prog);

    GLint view_loc = glGetUniformLocation(shader_prog, "View");
    GLint proj_loc = glGetUniformLocation(shader_prog, "Projection");

    auto proj = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);

    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(m_view_matrix));

    // glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

    glViewport(0, 0, 1920, 1080);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, m_index_buff_pos, GL_UNSIGNED_INT, NULL);
    SDL_GL_SwapWindow(m_window);
}
