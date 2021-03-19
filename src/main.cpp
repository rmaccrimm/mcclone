/*
 * 2017 SDL2 with OpenGL Boilerplate Code
 * Use this as you wish. Licensed under CC0. No attribution required.
 */
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include "RenderSystem.h"
#include "WorldChunk.h"

#include <glm/vec3.hpp>

/*
 * Program entry point
 */
int main(int argc, char *argv[])
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
			 SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    if (window == NULL) {
	fprintf(stderr, "Failed to create main window\n");
	SDL_Quit();
	return 1;
    }

    RenderSystem render(NULL, window);
    
    if (render.Initialize()) {
        return 1;
    }

    WorldChunk chunk;

    printf("Running...\n");
    render.RenderChunk(&chunk);
    render.Tick();
    
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
