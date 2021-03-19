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

#include "RenderSystem.h"
#include "EntityAdmin.h"
#include "WorldChunk.h"


const char *vert_shader_src = "\
#version 330 core                                                            \n\
layout(location=0) in vec3 in_Position;                                      \n\
uniform mat4 View;                                                           \n\
uniform mat4 Projection;                                                     \n\
void main()                                                                  \n\
{                                                                            \n\
    gl_Position = Projection * View * vec4(in_Position, 1.0);                \n\
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

const glm::vec3 cube_left[] = {
    glm::vec3(0, 0, 0),                             
    glm::vec3(0, 0, 1),                             
    glm::vec3(0, 1, 0),                             
    glm::vec3(0, 1, 1),                             
  
};
const glm::vec3 cube_right[] = {
    glm::vec3(1, 0, 1),
    glm::vec3(1, 0, 0),
    glm::vec3(1, 1, 1),
    glm::vec3(1, 1, 0),
};

const glm::vec3 cube_bottom[] = {
    glm::vec3(0, 0, 0),
    glm::vec3(1, 0, 0),
    glm::vec3(0, 0, 1),
    glm::vec3(1, 0, 1),
};

const glm::vec3 cube_top[] = {
    glm::vec3(1, 1, 0),
    glm::vec3(0, 1, 0),
    glm::vec3(1, 1, 1),
    glm::vec3(0, 1, 1),
};

const glm::vec3 cube_back[] = {
    glm::vec3(0, 0, 0),
    glm::vec3(0, 1, 0),
    glm::vec3(1, 0, 0),
    glm::vec3(1, 1, 0),
};

const glm::vec3 cube_front[] {
    glm::vec3(0, 0, 1),
    glm::vec3(0, 1, 1),
    glm::vec3(1, 0, 1),
    glm::vec3(1, 1, 1) 
};

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
int RenderSystem::Initialize()
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

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    InitShaders();
    InitGeometry();
    // InitTextures();

  return 0;
}

/*
 * Initialize Shaders
 */
int RenderSystem::InitShaders()
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

float vert_buff[100000 * 3] = {0};
int index_buff[100000 * 3] = {0};

/*
 * Initialize Geometry
 */
int RenderSystem::InitGeometry()
{
    // Populate vertex buffer
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert_buff), NULL, GL_STREAM_DRAW);

    // Populate element buffer
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buff), NULL, GL_STREAM_DRAW);

    // Bind vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    return 0;
}


RenderSystem::RenderSystem(EntityAdmin *admin, SDL_Window* window) {
    m_admin = admin;
    m_window = window;
}

RenderSystem::~RenderSystem() {
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
}



void RenderSystem::RenderChunk(WorldChunk *chunk) {
    int i = 0;
    int k = 0;
    int z = 0;
    for (int x = 0; x < WorldChunk::SPAN_X; x++) {
	for (int z = 0; z < WorldChunk::SPAN_Z; z++) {
	    if (chunk->m_blocks[x][0][z]) {

		glm::vec3 position = glm::vec3((float)x, 0, (float)z);
		glm::mat4 translate = glm::translate(glm::mat4(1), position);

		// std::cout << "i: " << i << std::endl;
		// std::cout << "Drawing block at: ("
		// 	  << x
		// 	  << ", "
		// 	  << z
		// 	  << ") "
		// 	  // << glm::to_string(translate * glm::vec4(cube_bottom[0].x,
		// 						  // cube_bottom[0].y,
		// 						  // cube_bottom[0].z,
		// 						  // 1))
		// 	  << glm::to_string(translate * glm::vec4(cube_bottom[0], 1))
		// 	  << "\n";
		

		index_buff[k++] = i/3;
		index_buff[k++] = i/3 + 3;
		index_buff[k++] = i/3 + 1;
		index_buff[k++] = i/3;
		index_buff[k++] = i/3 + 2;
		index_buff[k++] = i/3 + 3;
		for (int j = 0; j < 4; j++) {
		    glm::vec4 pos = translate * glm::vec4(cube_top[j], 1);
		    vert_buff[i++] = pos.x;
		    vert_buff[i++] = pos.y;
		    vert_buff[i++] = pos.z;
		}

		index_buff[k++] = i/3;
		index_buff[k++] = i/3 + 3;
		index_buff[k++] = i/3 + 1;
		index_buff[k++] = i/3;
		index_buff[k++] = i/3 + 2;
		index_buff[k++] = i/3 + 3;
		for (int j = 0; j < 4; j++) {
		    glm::vec4 pos = translate * glm::vec4(cube_bottom[j], 1);
		    vert_buff[i++] = pos.x;
		    vert_buff[i++] = pos.y;
		    vert_buff[i++] = pos.z;
		}

		index_buff[k++] = i/3;
		index_buff[k++] = i/3 + 3;
		index_buff[k++] = i/3 + 1;
		index_buff[k++] = i/3;
		index_buff[k++] = i/3 + 2;
		index_buff[k++] = i/3 + 3;
		for (int j = 0; j < 4; j++) {
		    glm::vec4 pos = translate * glm::vec4(cube_left[j], 1);
		    vert_buff[i++] = pos.x;
		    vert_buff[i++] = pos.y;
		    vert_buff[i++] = pos.z;
		}

		index_buff[k++] = i/3;
		index_buff[k++] = i/3 + 3;
		index_buff[k++] = i/3 + 1;
		index_buff[k++] = i/3;
		index_buff[k++] = i/3 + 2;
		index_buff[k++] = i/3 + 3;
		for (int j = 0; j < 4; j++) {
		    glm::vec3 pos = translate * glm::vec4(cube_right[j], 1);
		    vert_buff[i++] = pos.x;
		    vert_buff[i++] = pos.y;
		    vert_buff[i++] = pos.z;
		}

		index_buff[k++] = i/3;
		index_buff[k++] = i/3 + 3;
		index_buff[k++] = i/3 + 1;
		index_buff[k++] = i/3;
		index_buff[k++] = i/3 + 2;
		index_buff[k++] = i/3 + 3;
		for (int j = 0; j < 4; j++) {
		    glm::vec4 pos = translate * glm::vec4(cube_front[j], 0);
		    vert_buff[i++] = pos.x;
		    vert_buff[i++] = pos.y;
		    vert_buff[i++] = pos.z;
		}

		index_buff[k++] = i/3;
		index_buff[k++] = i/3 + 3;
		index_buff[k++] = i/3 + 1;
		index_buff[k++] = i/3;
		index_buff[k++] = i/3 + 2;
		index_buff[k++] = i/3 + 3;
		for (int j = 0; j < 4; j++) {
		    glm::vec4 pos = translate * glm::vec4(cube_back[j], 1);
		    vert_buff[i++] = pos.x;
		    vert_buff[i++] = pos.y;
		    vert_buff[i++] = pos.z;
		}
	    }
	}
	
    }
    
    // for (int q = 0; q < i; q++) {
	// if (!(q % 3)) {
	    // std::cout << '\n';
	// }
	// std::cout << vert_buff[q] << ' ';
    // }

    glBufferSubData(GL_ARRAY_BUFFER,
		    0,
		    (i) * sizeof(GLfloat) ,
		    vert_buff);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
		    0,
		    (k) * sizeof(GLint),
		    index_buff);

    glUseProgram(m_shader_prog);

    GLint view_loc = glGetUniformLocation(m_shader_prog, "View");
    GLint proj_loc = glGetUniformLocation(m_shader_prog, "Projection");

    auto proj = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    auto view =
	glm::lookAt(glm::vec3(-5.0f, 10.0f, -4.0f), glm::vec3(16.0f, 0.0f, 16.0f),
		    glm::vec3(0.0f, 1.0f, 0.0));

    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, k, GL_UNSIGNED_INT, NULL);
    SDL_GL_SwapWindow(m_window);
    
}

void RenderSystem::Tick() {
    
}
