#ifndef _BOXCOLLIDERMOVEMENTSYSTEM_H_
#define _BOXCOLLIDERMOVEMENTSYSTEM_H_

#include "System.h"
#include <glm/vec3.hpp>

bool detectCollision(glm::vec3 a_pos, glm::vec3 a_size, glm::vec3 b_pos, glm::vec3 b_size);

glm::vec3 resolveCollision(
    glm::vec3 moving_p0,
    glm::vec3 moving_p1,
    glm::vec3 moving_size,
    glm::vec3 immovable_pos,
    glm::vec3 immovable_size);

class BoxColliderMovementSystem : public System {
public:
    BoxColliderMovementSystem(EntityAdmin& admin) : System(admin) { }
    void tick() override;
};

#endif /* _BOXCOLLIDERMOVEMENTSYSTEM_H_ */
