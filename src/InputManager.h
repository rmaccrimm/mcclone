#ifndef _INPUTMANAGER_H_
#define _INPUTMANAGER_H_

#include <cstdint>
#include <map>
#include <SDL2/SDL.h>

enum Key {
    LEFT,
    RIGHT,
    FORWARD,
    BACK,
    UP,
    DOWN
};

// Will probably split into a component/system later
class InputManager
{
public:
    InputManager(SDL_Window* window);
    void update(SDL_Event &event);
    bool isPressed(Key);

private:
    bool m_key_pressed[6];
    SDL_Window *m_window;
};

#endif /* _INPUTMANAGER_H_ */
