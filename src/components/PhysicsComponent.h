#ifndef _PHYSICSCOMPONENT_H_
#define _PHYSICSCOMPONENT_H_

#include <glm/vec3.hpp>

struct PhysicsComponent
{
    static const int TAG = 1 << __COUNTER__;
    
    float accel_rate = 0.1;
    float deccel_rate;
    float max_speed;
    bool feels_gravity;
    glm::vec3 velocity;
};


#endif /* _PHYSICSCOMPONENT_H_ */
