#include "MovementSystem.h"
#include "EntityAdmin.h"
#include "components/Component.h"

MovementSystem::MovementSystem(EntityAdmin* admin) : m_admin { admin } { }

void MovementSystem::tick()
{
    for (auto id : m_admin->componentView<PhysicsComponent, TransformComponent>()) {
	auto physics = m_admin->getComponent<PhysicsComponent>(id);
	auto transform = m_admin->getComponent<TransformComponent>(id);

	float dt = 1.0f / 60.0f;
	transform->m_position += dt * physics->velocity;
    }
}
