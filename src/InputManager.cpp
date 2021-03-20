#include "InputManager.h"


void InputManager::update() {
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        auto key_code = event.key.keysym.sym;
        if (key_code == SDLK_ESCAPE) {
            quit = true;
            return;
        }
        if (key_map.find(key_code) == key_map.end()) {
            return;
        }
        int joy_key = key_map.at(key_code);
        
        if (event.type == SDL_KEYDOWN) {
            if (!key_pressed[key_code]) {
                switch (key_code) 
                {
                case SDLK_p:
                    pause = true;
                    break;
                case SDLK_1:
                case SDLK_2:
                case SDLK_3:
                case SDLK_4:
                case SDLK_5:
                case SDLK_6:
                case SDLK_7:
                case SDLK_8:
                case SDLK_9:
                    current_palette = key_code - SDLK_1;
                    break;
                case SDLK_BACKQUOTE:
                    invert_colors = !invert_colors;
                };
                key_pressed[key_code] = !key_pressed[key_code];
                joypad->press_key(joy_key);
            }
        }
        else {
          if (key_pressed[key_code]) {
            joypad->release_key(joy_key);
            key_pressed[key_code] = !key_pressed[key_code];
          }
        }
    }
    draw = false;
}
