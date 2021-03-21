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
#include "InputManager.h"
#include "WorldChunk.h"
#include "EntityAdmin.h"
#include "components/Component.h"

#include <unordered_map>
#include <ranges>

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

    // Create main window
    SDL_Window *window = 
	SDL_CreateWindow("SDL App", SDL_WINDOWPOS_CENTERED,
			 SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_OPENGL);
    if (window == NULL) {
	fprintf(stderr, "Failed to create main window\n");
	SDL_Quit();
	return 1;
    }

    
    Renderer render(window);
    InputManager input_mgr(window);
    EntityAdmin admin(&renderer);
    admin.createEntity<CameraComponent, TransformComponent, PlayerControlComponent>();

    // return 0;
    
    if (render.initialize()) {
        return 1;
    }

    WorldChunk chunk;

    printf("Running...\n");
    render.renderChunk(&chunk);

    
    for (should_run = 1; should_run; ) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
                should_run = 0;
                break;
            }
        }
    }

    printf("Exiting...\n");
    return 0;
}
