/*
 * 2017 SDL2 with OpenGL Boilerplate Code
 * Use this as you wish. Licensed under CC0. No attribution required.
 */
#include <SDL2/SDL.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>
#include <stdio.h>
#include <chrono>
#include <thread>

#include "ChunkManager.h"
#include "EntityAdmin.h"
#include "InputManager.h"
#include "SystemList.h"
#include "WorldChunk.h"
#include "components/Component.h"
#include "rendering/Renderer.h"

int initPlayer(EntityAdmin& admin, int argc, char** argv)
{
    int player_id = admin.createEntity<
        TransformComponent,
        PlayerControlComponent,
        MovementComponent,
        PhysicsComponent>();
    admin.getComponent<TransformComponent>(player_id).m_position.y = 40.0f;
    if (argc == 5) {
        auto& physics = admin.getComponent<PhysicsComponent>(player_id);
        physics.accel_rate = std::stoi(argv[2]);
        physics.drag = std::stoi(argv[3]);
        physics.max_speed = std::stoi(argv[4]);
    }
    return player_id;
}

int main(int argc, char** argv)
{
    // Initialize logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
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

    Renderer renderer(window);
    if (renderer.initialize()) {
        return 1;
    }

    InputManager input_mgr(window);
    ChunkManager chunk_mgr(renderer, argc > 1 ? std::stoi(argv[1]) : 5);
    EntityAdmin admin(&input_mgr, &renderer, &chunk_mgr);

    SystemList system_list(admin);
    
    int player_id = initPlayer(admin, argc, argv);

    int camera_id
        = admin.createEntity<CameraComponent, CameraControlComponent, TransformComponent>();
    admin.getComponent<CameraComponent>(camera_id).target = player_id;

    auto transform = admin.getComponent<TransformComponent>(player_id);
    chunk_mgr.reloadChunks(transform.m_position);


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
	system_list.runAll();
        renderer.draw();

        auto t_draw = steady_clock::now();
        dt = duration_cast<duration<double>>(t_draw - t);
        if (dt < T) {
            milliseconds pause = duration_cast<milliseconds>(T - dt);
            std::this_thread::sleep_for(pause);
        }
        t = steady_clock::now();
    }

    SDL_DestroyWindow(window);

    LOG_INFO << "Exiting";
    return 0;
}
