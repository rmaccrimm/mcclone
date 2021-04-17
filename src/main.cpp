/*
 * 2017 SDL2 with OpenGL Boilerplate Code
 * Use this as you wish. Licensed under CC0. No attribution required.
 */
#include <GL/glew.h>
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
#include "ChunkManager.h"
#include "EntityAdmin.h"
#include "InputManager.h"
#include "WorldChunk.h"
#include "components/Component.h"
#include "rendering/RenderSystem.h"
#include "rendering/Renderer.h"

#include <chrono>
#include <ranges>
#include <thread>
#include <unordered_map>

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

    // printf("Initializing TTF\n");
    // if(TTF_Init()==-1) {
    // 	printf("TTF_Init: %s\n", TTF_GetError());
    // 	SDL_Quit();
    // 	return 1;
    // }
    // TTF_Font* font = TTF_OpenFont("/usr/share/fonts/liberation/LiberationSans-Regular.ttf", 24);
    // if (!font) {
    // 	printf("TTF_Init: %s\n", TTF_GetError());
    // 	SDL_Quit();
    // 	return 1;
    // }

    // SDL_Color White = {255, 255, 255};
    // SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "put your text here", White);
    // printf("Rendered surface\n");

    // SDL_FreeSurface(surfaceMessage);
    // TTF_CloseFont(font);
    // SDL_DestroyWindow(window);
    // SDL_Quit();

    // return 0;

    Renderer renderer(window);
    InputManager input_mgr(window);
    ChunkManager chunk_mgr;
    EntityAdmin admin(&input_mgr, &renderer, &chunk_mgr);
    int camera_id
        = admin.createEntity<CameraComponent, TransformComponent, PlayerControlComponent>();

    auto transform = admin.getComponent<TransformComponent>(camera_id);
    transform->m_position = glm::vec3(16, 40.0, -10.0);
    transform->m_forward = glm::vec3(16.0, 10.0, 16.0) - transform->m_position;

    CameraMovementSystem cam_system(&admin);
    RenderSystem render_system(&admin);

    if (renderer.initialize()) {
        return 1;
    }

    render_system.tick();

    using namespace std::chrono;
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

        renderer.draw();

        auto t_draw = steady_clock::now();
        dt = duration_cast<duration<double>>(t_draw - t);
        if (dt < T) {
            milliseconds pause = duration_cast<milliseconds>(T - dt);
            std::this_thread::sleep_for(pause);
        }
        t = steady_clock::now();
    }

    LOG_INFO << "Exiting";
    return 0;
}
