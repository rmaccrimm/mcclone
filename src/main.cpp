/*
 * 2017 SDL2 with OpenGL Boilerplate Code
 * Use this as you wish. Licensed under CC0. No attribution required.
 */
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/FuncMessageFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>
#include <stdio.h>
#include <variant>

#include "CameraMovementSystem.h"
#include "ChunkLoadingSystem.h"
#include "ChunkManager.h"
#include "EntityAdmin.h"
#include "InputManager.h"
#include "WorldChunk.h"
#include "components/Component.h"
#include "ChunkRenderSystem.h"
#include "rendering/Renderer.h"

#include <chrono>
#include <ranges>
#include <thread>
#include <unordered_map>

void flip_surface(SDL_Surface* surface)
{
    SDL_LockSurface(surface);

    int pitch = surface->pitch; // row size
    char* temp = new char[pitch]; // intermediate buffer
    char* pixels = (char*)surface->pixels;

    for (int i = 0; i < surface->h / 2; ++i) {
        // get pointers to the two rows to swap
        char* row1 = pixels + i * pitch;
        char* row2 = pixels + (surface->h - i - 1) * pitch;

        // swap rows
        memcpy(temp, row1, pitch);
        memcpy(row1, row2, pitch);
        memcpy(row2, temp, pitch);
    }
    
    for (int i = 0; i < surface->h * surface->w; i++) {
	int p = pixels[i];
	unsigned int b = (p >> 24) & 0xff;
	unsigned int g = (p >> 16) & 0xff;
	unsigned int r = (p >> 8) & 0xff;
	unsigned int a = p & 0xff;
	pixels[i] = r << 24 | g << 16 | b << 8 | a;
    }

    delete[] temp;

    SDL_UnlockSurface(surface);
}

int main()
{
    // Initialize logger
    static plog::ColorConsoleAppender<plog::FuncMessageFormatter> consoleAppender;
    plog::init(plog::verbose, &consoleAppender);

    int should_run;

    LOG_INFO << "Initializing SDL";
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG_ERROR << "Failed to initialize SDL video";
        return 1;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_Window* window = SDL_CreateWindow(
        "SDL App", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_OPENGL);
    if (window == NULL) {
        LOG_ERROR << "Failed to create main window";
        SDL_Quit();
        return 1;
    }

    LOG_INFO << "Initializing TTF";
    if (TTF_Init() == -1) {
        LOG_ERROR << "TTF_Init: " << TTF_GetError();
        SDL_Quit();
        return 1;
    }
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/TTF/FiraCode-Regular.ttf", 16);
    if (!font) {
        LOG_ERROR << "TTF_Init: " << TTF_GetError();
        SDL_Quit();
        return 1;
    }

    LOG_INFO << "Rendering text";
    SDL_Color white = { 255, 255, 255, 255 };

    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(
        font, "Here's some text here. Yep, just a whole lot of text baybeeeeee", white, 250);
    TTF_CloseFont(font);
    if (surface == nullptr) {
        LOG_ERROR << "Failed to render text to surface";
        SDL_Quit();
        return 1;
    }
    flip_surface(surface);
    LOG_INFO << "Created surface with format " << SDL_GetPixelFormatName(surface->format->format);

    Renderer renderer(window);
    if (renderer.initialize()) {
        return 1;
    }
    
    InputManager input_mgr(window);
    ChunkManager chunk_mgr(&renderer);

    EntityAdmin admin(&input_mgr, &renderer, &chunk_mgr);
    int camera_id
        = admin.createEntity<CameraComponent, TransformComponent, PlayerControlComponent>();

    auto transform = admin.getComponent<TransformComponent>(camera_id);
    transform->m_position = glm::vec3(16, 40.0, -10.0);
    transform->m_forward = glm::vec3(16.0, 10.0, 16.0) - transform->m_position;

    chunk_mgr.reloadChunks(transform->m_position);

    CameraMovementSystem cam_system(&admin);
    ChunkRenderSystem render_system(&admin);
    ChunkLoadingSystem load_system(&admin);

    using namespace std::chrono;
    
    auto t0 = high_resolution_clock::now();
    render_system.tick();
    auto t1 = high_resolution_clock::now();
    duration<double> tdiff = t1 - t0;
    
    LOG_INFO << "Took " << tdiff.count() << " seconds to load world";

    double framerate = 60;
    duration<double> T(1.0 / framerate);
    duration<double> dt;
    steady_clock::time_point t = steady_clock::now();

    SDL_Event event;
    LOG_INFO << "Starting main loop";
    for (should_run = 1; should_run;) {
        input_mgr.reset();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
                should_run = 0;
                break;
            }
            input_mgr.update(event);
        }
        cam_system.tick();
	load_system.tick();
	render_system.tick();
        renderer.draw();

        auto t_draw = steady_clock::now();
        dt = duration_cast<duration<double>>(t_draw - t);
        if (dt < T) {
            milliseconds pause = duration_cast<milliseconds>(T - dt);
            std::this_thread::sleep_for(pause);
        }
        t = steady_clock::now();
    }

    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);

    LOG_INFO << "Exiting";
    return 0;
}
