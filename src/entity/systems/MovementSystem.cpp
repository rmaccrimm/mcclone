#include "MovementSystem.h"
#include "EntityAdmin.h"
#include "components/Component.h"

void MovementSystem::tick()
{
    for (int id :
         m_admin.componentView<PhysicsComponent, TransformComponent, BoxColliderComponent>()) {
        auto physics = m_admin.getComponent<PhysicsComponent>(id);
	auto collider = m_admin.getComponent<BoxColliderComponent>(id);
        auto& transform = m_admin.getComponent<TransformComponent>(id);

	if (!collider.collision_detected) {
	    float dt = 1.0f / 60.0f;
	    transform.m_position += dt * physics.velocity;
	}
    }
}
