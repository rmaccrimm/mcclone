#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <string>
#include "System.h"

class EntityAdmin;

class RenderSystem : public System {
public:
    RenderSystem(EntityAdmin *admin);
    ~RenderSystem();

    void Tick();

    void draw_square();

private:
    int m_SCREEN_W;
    int m_SCREEN_H;
    int m_window_scale;
    SDL_GLContext m_gl_context;
    std::string m_title;
    SDL_Window *m_sdl_window;
    EntityAdmin *m_admin;
};

#endif
