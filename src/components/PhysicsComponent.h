#ifndef _PHYSICSCOMPONENT_H_
#define _PHYSICSCOMPONENT_H_

#include <glm/vec3.hpp>

struct PhysicsComponent
{
    static const int TAG = 1 << 6;
    
    float accel_rate = 300;
    float drag = 200;
    float max_speed = 50;
    bool feels_gravity = true;
    glm::vec3 velocity = {0, 0, 0};
};


#endif /* _PHYSICSCOMPONENT_H_ */
