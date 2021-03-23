#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <vector>

class WorldChunk;

class Renderer {
    
public:
    Renderer(SDL_Window *window);
    ~Renderer();

    int initialize();

    void draw();

    void clearBuffers();
    
    void renderChunk(WorldChunk* chunk);
    
    void setViewMatrix(glm::mat4 view_matrix);

    void copyVertexData(const std::vector<glm::vec3> &verts,
			const std::vector<int> &inds);

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
    
    std::vector<GLfloat> m_vert_buff;
    std::vector<GLint> m_index_buff;
    std::vector<GLfloat>::iterator m_vert_iter;
    std::vector<GLint>::iterator m_index_iter;
    
    glm::mat4 m_view_matrix;
};

#endif /* _RENDERER_H_ */
