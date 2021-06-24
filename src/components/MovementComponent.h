#ifndef _MOVEMENTCOMPONENT_H_
#define _MOVEMENTCOMPONENT_H_

#include <glm/vec3.hpp>

// Indicates movement "intent" before letting the physics system handle the actual movement
struct MovementComponent
{
    static const int TAG = 1 << __COUNTER__;
    glm::vec3 direction;
};


#endif /* _MOVEMENTCOMPONENT_H_ */
