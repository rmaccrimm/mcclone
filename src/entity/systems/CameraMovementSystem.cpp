#include "CameraMovementSystem.h"
#include "EntityAdmin.h"
#include "InputManager.h"
#include "components/Component.h"
#include "rendering/Renderer.h"
#include "util/math.h"

#include <glm/gtc/constants.hpp>

CameraMovementSystem::CameraMovementSystem(EntityAdmin* admin) : m_admin(admin) { }

// Not sure if there's really any reason for this to be separate from CameraControlSystem
void CameraMovementSystem::tick()
{
    auto input = m_admin->getInputManager();
    auto renderer = m_admin->getRenderer();
    for (int id :
         m_admin->componentView<CameraComponent, TransformComponent>()) {
	auto cam = m_admin->getComponent<CameraComponent>(id);
	auto transform = m_admin->getComponent<TransformComponent>(id);

	// Camera just directly inherits position from its' target
	transform->m_position = m_admin->getComponent<TransformComponent>(cam->target)->m_position;
	
	// Update the rendering matrices
        glm::mat4 view = glm::lookAt(
            transform->m_position, transform->m_position + transform->m_forward, transform->m_up);
        renderer->setViewMatrix(view);
    }
}
