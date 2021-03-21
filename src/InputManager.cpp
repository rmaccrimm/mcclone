#include "InputManager.h"
#include <SDL2/SDL.h>
#include <iostream>

static const std::map<SDL_Keycode, Key> key_map = {
    {SDLK_a, LEFT},
    {SDLK_d, RIGHT},
    {SDLK_w, FORWARD},
    {SDLK_s, BACK},
    {SDLK_LSHIFT, DOWN},
    {SDLK_SPACE, UP}
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
        int k = key_map.at(key_code);
        
        if (event.type == SDL_KEYDOWN) {
            if (!m_key_pressed[k]) {
		std::cout << "Pressed key " << k << std::endl;
                m_key_pressed[k] = !m_key_pressed[k];
            }
        }
        else {
          if (m_key_pressed[k]) {
	      std::cout << "Released key " << k << std::endl;
	      m_key_pressed[k] = !m_key_pressed[k];
          }
        }
    }
}

bool InputManager::isPressed(Key k)
{
    return m_key_pressed[k];
}
