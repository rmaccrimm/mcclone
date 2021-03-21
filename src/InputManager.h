#ifndef _INPUTMANAGER_H_
#define _INPUTMANAGER_H_

#include <cstdint>
#include <map>

class SDL_Window;
class SDL_Event;

enum class Key {
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
    void update(SDL_Event &event);
    
    InputManager(SDL_Window* window);
    bool isPressed(Key);

private:
    bool m_key_pressed[6];
    SDL_Window *m_window;
};

#endif /* _INPUTMANAGER_H_ */
