#ifndef _RENDERSYSTEM_H_
#define _RENDERSYSTEM_H_

#include <SDL2/SDL.h>
#include <GL/glew.h>


class WorldChunk;

class Renderer {
    
public:
    Renderer(SDL_Window *window);
    ~Renderer();

    int initialize();
    void tick();
    void renderChunk(WorldChunk* chunk);

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
    GLfloat *m_vert_buff;
    GLint *m_index_buff;
};

#endif /* _RENDERSYSTEM_H_ */
