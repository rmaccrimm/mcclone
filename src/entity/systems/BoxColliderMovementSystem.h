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

private:
    glm::vec3 calcVelocity(glm::vec3 p_init, glm::vec3 size, glm::vec3 velocity);
    glm::vec3 calcFinalPosition(glm::vec3 p_init, glm::vec3 p_final, glm::vec3 size);
};

#endif /* _BOXCOLLIDERMOVEMENTSYSTEM_H_ */
