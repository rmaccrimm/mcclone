#include "PhysicsSystem.h"
#include "EntityAdmin.h"
#include "components/Component.h"

#include <glm/geometric.hpp>

PhysicsSystem::PhysicsSystem(EntityAdmin* admin) : m_admin { admin } { }

void PhysicsSystem::tick()
{
    for (auto id : m_admin->componentView<MovementComponent, PhysicsComponent>()) {
	auto move = m_admin->getComponent<MovementComponent>(id);
	auto physics = m_admin->getComponent<PhysicsComponent>(id);

	glm::vec3 dir = physics->accel_rate * glm::normalize(move->direction);
	if (dir == glm::vec3(0, 0, 0)) {
	    // start slowing down
	    physics->velocity = glm::vec3(0, 0, 0);
	}
	else {
	    float dt = 1.0f/60.0f; // Get this from timer
	    physics->velocity += dt * dir; // Limit this by some max velocity
	}
    }
}
