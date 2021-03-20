#ifndef _INPUTMANAGER_H_
#define _INPUTMANAGER_H_

#include <cstdint>

class InputManager
{
public:
    enum Key {
	LEFT,
	RIGHT,
	FORWARD,
	BACK,
	UP,
	DOWN
    };

    void update();
    
    InputManager();
    bool isPressed(Key);

private:
    uint32_t m_button_state;
};


#endif /* _INPUTMANAGER_H_ */
