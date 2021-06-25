#include "PhysicsSystem.h"
#include "EntityAdmin.h"
#include "components/Component.h"

#include <algorithm>
#include <glm/geometric.hpp>

void PhysicsSystem::tick()
{
    for (auto id : m_admin.componentView<MovementComponent, PhysicsComponent>()) {
        auto move = m_admin.getComponent<MovementComponent>(id);
        auto& phys = m_admin.getComponent<PhysicsComponent>(id);

        const glm::vec3 zero(0, 0, 0);
        glm::vec3& v = phys.velocity;
        glm::vec3 dn = move.direction == zero ? zero : glm::normalize(move.direction);

        const float accel = phys.accel_rate;
        const float drag = phys.drag;
        const float dt = 1.0f / 60.0f; // TODO get this from timer

        auto drag_accel = (v == zero) ? zero : drag * -glm::normalize(v);
        auto input_accel = (dn == zero) ? zero : accel * dn;
        auto total_accel = drag_accel + input_accel;

        auto v_new = v + dt * total_accel;
        // If the drag acceleration starts moving obj in opposite direction, stop moving
        v = glm::dot(v, v_new) < 0 ? zero : v_new;

        float speed = glm::length(v);
        if (speed > phys.max_speed) {
            v *= phys.max_speed / speed;
        }
    }
}
