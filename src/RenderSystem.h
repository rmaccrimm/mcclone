#ifndef _RENDERSYSTEM_H_
#define _RENDERSYSTEM_H_

#include <SDL2/SDL.h>
#include <GL/glew.h>


class EntityAdmin;
class WorldChunk;


class RenderSystem {
    
public:
    RenderSystem(EntityAdmin *admin, SDL_Window *window);
    ~RenderSystem();

    int Initialize();
    void Tick();
    void RenderChunk(WorldChunk* chunk);

private:
    int Update();
    int Cleanup();
    int InitShaders();
    int InitGeometry();
    int InitTextures();


    // void RenderEntities();
    
    EntityAdmin *m_admin;
    
    int m_SCREEN_W;
    int m_SCREEN_H;
    
    SDL_Window* m_window;
    SDL_GLContext m_context;
    GLuint m_vao, m_vbo, m_ebo, m_tex;
    GLuint m_vert_shader;
    GLuint m_frag_shader;
    GLuint m_shader_prog;
};

#endif /* _RENDERSYSTEM_H_ */
