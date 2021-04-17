#include "InputManager.h"
#include <SDL2/SDL.h>
#include <plog/Log.h>

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

void InputManager::reset()
{
    m_mouse_delta_x = 0;
    m_mouse_delta_y = 0;
}

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
		LOG_VERBOSE << "Pressed key " << k;
                m_key_pressed[k] = !m_key_pressed[k];
            }
        }
        else {
          if (m_key_pressed[k]) {
	      LOG_VERBOSE << "Released key " << k;
	      m_key_pressed[k] = !m_key_pressed[k];
          }
        }
    }
    if (event.type == SDL_MOUSEMOTION) {
	m_mouse_delta_x += event.motion.xrel;
	m_mouse_delta_y += event.motion.yrel;
    }
}

bool InputManager::isPressed(Key k)
{
    return m_key_pressed[k];
}
