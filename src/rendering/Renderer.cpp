#include <fstream>
#include <streambuf>
#include <string>
#include <vector>

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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ChunkManager.h"
#include "EntityAdmin.h"
#include "Renderer.h"
#include "WorldChunk.h"
#include "config.h"

const int VERT_BUFF_SIZE = 500 * (1 << 20);
const int INDEX_BUFF_SIZE = 250 * (1 << 20);

// clang-format off
float screen_quad[][5] = {
//   position    texture coords
    {-1,  1, 0,   0, 1},
    {-1, -1, 0,   0, 0},
    { 1, -1, 0,   1, 0},
    {-1,  1, 0,   0, 1},
    { 1, -1, 0,   1, 0},
    { 1,  1, 0,   1, 1}
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
    glDeleteTextures(1, &m_screen.tex);
    glDeleteBuffers(1, &m_scene.ebo);
    glDeleteBuffers(1, &m_scene.vbo);
    glDeleteBuffers(1, &m_screen.ebo);
    glDeleteBuffers(1, &m_screen.vbo);
    glDeleteVertexArrays(1, &m_scene.vao);
    glDeleteVertexArrays(1, &m_screen.vao);
    glDeleteFramebuffers(1, &m_frame_buffer);
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
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
    LOG_INFO << "Initializing GLEW";
    GLenum err;
    glewExperimental = GL_TRUE; // expose OpenGL 3.x+ interfaces
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

    LOG_INFO << "Registered shader " << '"' << shader_name << '"';

    return 0;
}

/*
 * Initialize Geometry
 */
int Renderer::initGeometry()
{
    // TODO - somehow make this more generic? Might only need 2 vaos though
    glGenVertexArrays(1, &m_scene.vao);
    glGenVertexArrays(1, &m_screen.vao);

    /*
      Main scene rendering vertex data
    */
    glBindVertexArray(m_scene.vao);

    // Allocate vertex buffer
    glGenBuffers(1, &m_scene.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_scene.vbo);
    glBufferData(GL_ARRAY_BUFFER, VERT_BUFF_SIZE, NULL, GL_STREAM_DRAW);

    // Allocate element buffer
    glGenBuffers(1, &m_scene.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_scene.ebo);
    // buffer is initially empty
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDEX_BUFF_SIZE, NULL, GL_STREAM_DRAW);

    int stride = 9 * sizeof(GLfloat);
    // Bind vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // Bind vertex color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Bind vertex normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    /*
      Screen quad rendering vertex data
    */
    glBindVertexArray(m_screen.vao);
    glGenBuffers(1, &m_screen.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_screen.vbo);

    // Write quad vertex data to buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad), nullptr, GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // vertex texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return 0;
}

int Renderer::initTextures()
{
    LOG_INFO << "Creating screen texture";
    glGenTextures(1, &m_screen.tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_screen.tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    LOG_INFO << "Creating depth buffer";
    glGenTextures(1, &m_screen.depth_buffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_screen.depth_buffer);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1920, 1080, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    LOG_INFO << "Creating Framebuffer";

    glGenFramebuffers(1, &m_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_screen.tex, 0);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_screen.depth_buffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR << "Framebuffer is not complete!";
        return 1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

void Renderer::draw(SDL_Surface* surface)
{
    /*
       First pass - render to texture
    */
    glBindVertexArray(m_scene.vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_scene.vbo);
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

    glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

    glViewport(0, 0, 1920, 1080);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // glDrawElements(GL_TRIANGLES, m_index_buff_pos, GL_UNSIGNED_INT, NULL);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*
       Second pass - draw screen quad with rendered texture
    */
    glViewport(0, 0, 1920, 1080);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(m_screen.vao);
    auto shader = m_shader_prog_map["screen"];
    glUseProgram(shader);

    GLint texture_loc = glGetUniformLocation(shader, "screen_texture");
    glUniform1i(texture_loc, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_screen.tex);

    // LOG_DEBUG << "w: " << (float)surface->w;
    // LOG_DEBUG << "h: " << (float)surface->h;
    
    // Write quad vertex data to buffer
    for (int i = 0; i < 6; i++) {
	screen_quad[i][3] = screen_quad[i][3] == 0 ? 0 : 1920.0f / (float)surface->w;
	screen_quad[i][4] = screen_quad[i][4] == 0 ? 0 : 1080.0f / (float)surface->h;
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_screen.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad), screen_quad, GL_STATIC_DRAW);

    SDL_LockSurface(surface);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        surface->w,
        surface->h,
        0,
        GL_RGBA,
        GL_UNSIGNED_INT_8_8_8_8_REV,
        surface->pixels);

    SDL_UnlockSurface(surface);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    SDL_GL_SwapWindow(m_window);
}
