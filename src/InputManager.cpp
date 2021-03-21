#include "InputManager.h"
#include <SDL2/SDL.h>

static const std::map<SDL_Keycode, Key> key_map = {
    {SDLK_LEFT, Key::LEFT},
    {SDLK_RIGHT, Key::RIGHT},
    {SDLK_UP, Key::UP},
    {SDLK_DOWN, Key::DOWN},
    // {SDLK_a, Joypad::A},
    // {SDLK_b, Joypad::B},
    // {SDLK_p, Joypad::NONE},
    // {SDLK_RETURN, Joypad::START},
    // {SDLK_BACKSPACE, Joypad::SELECT},
    // {SDLK_1, Joypad::NONE},
    // {SDLK_2, Joypad::NONE},
    // {SDLK_3, Joypad::NONE},
    // {SDLK_4, Joypad::NONE},
    // {SDLK_5, Joypad::NONE},
    // {SDLK_6, Joypad::NONE},
    // {SDLK_7, Joypad::NONE},
    // {SDLK_8, Joypad::NONE},
    // {SDLK_9, Joypad::NONE},
    // {SDLK_BACKQUOTE, Joypad::NONE}
};

InputManager::InputManager(SDL_Window *window)
    : m_key_pressed{0}, m_window{window} {}

void InputManager::update(SDL_Event &event)
{
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        auto key_code = event.key.keysym.sym;
        if (key_map.find(key_code) == key_map.end()) {
            return;
        }
        int joy_key = key_map.at(key_code);
        
        if (event.type == SDL_KEYDOWN) {
            if (!m_key_pressed[key_code]) {
                key_pressed[key_code] = !key_pressed[key_code];
            }
        }
        else {
          if (key_pressed[key_code]) {
            key_pressed[key_code] = !key_pressed[key_code];
          }
        }
    }
    draw = false;
}
