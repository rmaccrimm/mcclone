#include "CameraMovementSystem.h"
#include "EntityAdmin.h"
#include "components/Component.h"
#include "InputManager.h"
#include "rendering/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>


CameraMovementSystem::CameraMovementSystem(EntityAdmin *admin)
    : m_admin(admin) {}


float velocity = 0.1;

/* Probably want to split this into a player movement system that responds to input and
   doesn't need to know about the camera, and then a camera render system that uses
   the camera + transform to update the Renderer state
   - maybe better yet, a movement system with acceleration/velocity vectors and a player 
     input system that simply sets an indicator to start accelerating?
     -> something like an impulse component?
   - later there will probably be separate render systems for the world chunks and entities
     that pass vertex data into the renderer
*/
void CameraMovementSystem::tick() {
    auto input = m_admin->getInputManager();
    auto renderer = m_admin->getRenderer();
    // Don't know if it actually makes sense for there can be multiple of these yet
    for (int id: m_admin->componentView<
	     CameraComponent,
	     TransformComponent,
	     PlayerControlComponent>())
    {
	auto cam = m_admin->getComponent<CameraComponent>(id);
	auto transform = m_admin->getComponent<TransformComponent>(id);

	glm::vec3 pos = transform->m_position;
	glm::vec3 forward = transform->m_forward;
	glm::vec3 up = transform->m_up;
	glm::vec3 right = transform->m_right;

	if (input->isPressed(RIGHT)) {
	    transform->m_position = pos + velocity * right;
	}
	if (input->isPressed(LEFT)) {
	    transform->m_position = pos - velocity * right;
	}
	if (input->isPressed(FORWARD)) {
	    transform->m_position = pos + glm::cross(up, right) * velocity;
	}
	if (input->isPressed(BACK)) {
	    transform->m_position = pos + glm::cross(right, up) * velocity;
	}
	if (input->isPressed(UP)) {
	    transform->m_position = pos + velocity * up;
	}
	if (input->isPressed(DOWN)) {
	    transform->m_position = pos - velocity * up;
	}

	glm::mat4 view = glm::lookAt(pos, pos + forward, up);
	renderer->setViewMatrix(view);
    }
}
