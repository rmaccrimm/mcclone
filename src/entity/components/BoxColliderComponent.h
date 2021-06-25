#ifndef _BOXCOLLIDERCOMPONENT_H_
#define _BOXCOLLIDERCOMPONENT_H_

#include <glm/vec3.hpp>

struct BoxColliderComponent {
    static const int TAG = 1 << 4;

    bool collision_detected;
    glm::vec3 size = {0.5, 2.0, 0.5};

    struct {
        bool neg_x = false;
        bool neg_y = false;
        bool neg_z = false;
        bool pos_x = false;
        bool pos_y = false;
        bool pos_z = false;
    } collisions;
};

#endif /* _BOXCOLLIDERCOMPONENT_H_ */
