/*
 * 2017 SDL2 with OpenGL Boilerplate Code
 * Use this as you wish. Licensed under CC0. No attribution required.
 */
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <variant>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "rendering/Renderer.h"
#include "rendering/RenderSystem.h"
#include "InputManager.h"
#include "ChunkManager.h"
#include "WorldChunk.h"
#include "EntityAdmin.h"
#include "CameraMovementSystem.h"
#include "components/Component.h"

#include <unordered_map>
#include <ranges>
#include <chrono>
#include <thread>


/*
 * Program entry point
 */
int main()
{
    int should_run;

    printf("Initializing...\n");
    // Initialize SDL Video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
	fprintf(stderr, "Failed to initialize SDL video\n");
	return 1;
    }
    SDL_SetRelativeMouseMode(SDL_TRUE);
    // Create main window
    SDL_Window *window = 
	SDL_CreateWindow("SDL App", SDL_WINDOWPOS_CENTERED,
			 SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_OPENGL);
    if (window == NULL) {
	fprintf(stderr, "Failed to create main window\n");
	SDL_Quit();
	return 1;
    }

    
    Renderer renderer(window);
    InputManager input_mgr(window);
    ChunkManager chunk_mgr;
    EntityAdmin admin(&input_mgr, &renderer, &chunk_mgr);
    int camera_id = admin.createEntity<CameraComponent, TransformComponent, PlayerControlComponent>();

    auto transform = admin.getComponent<TransformComponent>(camera_id);
    transform->m_position = glm::vec3(16, 40.0, -10.0);
    transform->m_forward = glm::vec3(16.0, 10.0, 16.0) - transform->m_position;

    CameraMovementSystem cam_system(&admin);
    RenderSystem render_system(&admin);
    
    if (renderer.initialize()) {
        return 1;
    }

    printf("Transfering data to GPU...\n");

    render_system.tick();

    using namespace std::chrono;
    double framerate = 60;
    duration<double> T(1.0 / framerate);
    duration<double> dt;
    steady_clock::time_point t = steady_clock::now();

    SDL_Event event;
    printf("Starting main loop...\n");
    for (should_run = 1; should_run; ) {
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

    printf("Exiting...\n");
    return 0;
}
