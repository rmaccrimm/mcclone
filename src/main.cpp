/*
 * 2017 SDL2 with OpenGL Boilerplate Code
 * Use this as you wish. Licensed under CC0. No attribution required.
 */
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/scalar_constants.hpp>
#include "logo.h"

SDL_Window    *m_window;
SDL_GLContext  m_context;
GLuint         m_vao, m_vbo, m_ebo, m_tex;
GLuint         m_vert_shader;
GLuint         m_frag_shader;
GLuint         m_shader_prog;

int Initialize();
int Update();
int Cleanup();
int InitShaders();
int InitGeometry();
int InitTextures();

const char *vert_shader_src = "\
#version 330 core                                                            \n\
layout(location=0) in vec2 in_Position;                                      \n\
uniform mat4 View;                                                           \n\
uniform mat4 Projection;                                                     \n\
void main()                                                                  \n\
{                                                                            \n\
    gl_Position = Projection * View * vec4(in_Position, 0.0, 1.0);                \n\
}                                                                            \n\
";

const char *frag_shader_src = "\
#version 330 core                                                            \n\
//layout(location=0) out vec3 out_Color;                                       \n\
void main()                                                                  \n\
{                                                                            \n\
    gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);                                \n\
}                                                                            \n\
";

const GLfloat cube[24][3] = {
    // x = 0 face (left)
    {0, 0, 0},  // BL
    {0, 0, 1},  // BR
    {0, 1, 0},  // TL
    {0, 1, 1},  // TR
    // x = 1 face (right)
    {1, 0, 1},  // BL
    {1, 0, 0},  // BR 
    {1, 1, 1},  // TL
    {1, 1, 0},  // TR
    // y = 0 face (bottom)
    {0, 0, 0},  //
    {1, 0, 0},  //
    {0, 0, 1},  // 
    {1, 0, 1},  //
    // y = 1 face (top)
    {0, 1, 0},  //
    {0, 1, 1},  //
    {1, 1, 1},  //
    {1, 1, 0},  //
    // z = 0 face (back)
    {0, 0, 0},  //
    {0, 1, 0},  //
    {1, 0, 0},  //
    {1, 1, 0},  //
    // z = 1 face (front)
    {0, 0, 1},  //
    {0, 1, 1},  //
    {1, 0, 1},  //
    {1, 1, 1}   //
};

const GLfloat verts[6][2] = {
    //  x      y      s      t
    {-1.0f, -1.0f}, // BL
    {-1.0f, 1.0f},  // TL
    {1.0f, 1.0f},   // TR
    {1.0f, -1.0f},  // BR
};

const GLint indicies[] = {0, 1, 2, 0, 2, 3};

const GLint indices[] = {0,  3,  1,  0,  2,  3,  4,  7,  5,  4,  6,  7,
                         8,  11, 9,  8,  10, 11, 12, 15, 13, 12, 14, 15,
                         16, 19, 17, 16, 18, 19, 20, 23, 21, 20, 22, 23};

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
	     ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
	     type, severity, message );
}

/*
 * Basic Initialization
 */
int Initialize()
{
  // Initialize SDL Video
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      fprintf(stderr, "Failed to initialize SDL video\n");
      return 1;
  }

  // Create main window
  m_window =
      SDL_CreateWindow("SDL App", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
  if (m_window == NULL) {
      fprintf(stderr, "Failed to create main window\n");
      SDL_Quit();
      return 1;
  }

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

  InitShaders();
  InitGeometry();
  // InitTextures();

  return 0;
}

/*
 * Initialize Shaders
 */
int InitShaders()
{
    GLint status;
    char err_buf[512];

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Compile vertex shader
    m_vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vert_shader, 1, &vert_shader_src, NULL);
    glCompileShader(m_vert_shader);
    glGetShaderiv(m_vert_shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(m_vert_shader, sizeof(err_buf), NULL, err_buf);
        err_buf[sizeof(err_buf)-1] = '\0';
        fprintf(stderr, "Vertex shader compilation failed: %s\n", err_buf);
        return 1;
    }

    // Compile fragment shader
    m_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_frag_shader, 1, &frag_shader_src, NULL);
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
    // glBindFragDataLocation(m_shader_prog, 0, "out_Color");
    glLinkProgram(m_shader_prog);
    glUseProgram(m_shader_prog);

    return 0;
}

/*
 * Initialize Geometry
 */
int InitGeometry()
{
    // Populate vertex buffer
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Populate element buffer
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

    // Bind vertex position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // Bind vertex texture coordinate attribute
    // GLint tex_attr_loc = glGetAttribLocation(m_shader_prog, "in_Texcoord");
    // glVertexAttribPointer(tex_attr_loc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
    // glEnableVertexAttribArray(tex_attr_loc);

    return 0;
}

/*
 * Initialize textures
 */
int InitTextures()
{
    glGenTextures(1, &m_tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glUniform1i(glGetUniformLocation(m_shader_prog, "tex"), 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, logo_rgba);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return 0;
}

/*
 * Free resources
 */
int Cleanup()
{
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
    return 0;
}

/*
 * Render a frame
 */
int Update()
{
    glUseProgram(m_shader_prog);

    GLint view_loc = glGetUniformLocation(m_shader_prog, "View");
    GLint proj_loc = glGetUniformLocation(m_shader_prog, "Projection");

    auto proj = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    auto view = glm::lookAt(glm::vec3(5.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0));
    
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    SDL_GL_SwapWindow(m_window);
    return 0;
}

void CheckGL() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
	printf("\n A general  OGL error has occured: %i\n", err);
    }
}

/*
 * Program entry point
 */
int main(int argc, char *argv[])
{
    int should_run;

    printf("Initializing...\n");
    if (Initialize()) {
        return 1;
    }

    printf("Running...\n");
    for (should_run = 1; should_run; ) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
                should_run = 0;
                break;
            }
        }
	Update();
	// CheckGL();
    }

    printf("Exiting...\n");
    Cleanup();
    return 0;
}
