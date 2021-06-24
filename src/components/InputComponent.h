#ifndef _INPUTCOMPONENT_H_
#define _INPUTCOMPONENT_H_

#include <glm/vec3.hpp>

struct InputComponent
{
    static const int TAG = 1 << 5;    
    // Indicates the desired movement direction on each axis (-1, 0, 1)
    glm::ivec3 movement;
};


#endif /* _INPUTCOMPONENT_H_ */
