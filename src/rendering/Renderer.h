#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <memory>
#include <vector>
#include <array>

#include "Vertex.h"

class ChunkManager;

class Renderer {
    
public:
    Renderer(SDL_Window *window);
    ~Renderer();

    int initialize();

    void draw();

    void clearBuffers();
    
    void setViewMatrix(glm::mat4 view_matrix);

    void copyVertexData(const std::array<Vertex, 4> &verts,
			const std::array<int, 6> &inds);

    void reset();

private:
    int cleanup();
    int initShaders();
    int initGeometry();
    int initTextures();

    int m_SCREEN_W;
    int m_SCREEN_H;

    SDL_Window* m_window;
    SDL_GLContext m_context;
    GLuint m_vao, m_vbo, m_ebo, m_tex;
    GLuint m_vert_shader;
    GLuint m_frag_shader;
    GLuint m_shader_prog;
    
    GLuint m_frame_buffer;
    // TODO
    GLuint m_depth_buffer;
    
    std::unique_ptr<GLfloat[]> m_vert_buff;
    std::unique_ptr<GLint[]> m_index_buff;
    int m_vert_buff_pos;
    int m_index_buff_pos;
    
    glm::mat4 m_view_matrix;
};

#endif /* _RENDERER_H_ */
