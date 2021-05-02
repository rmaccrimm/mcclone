#include "CameraMovementSystem.h"
#include "EntityAdmin.h"
#include "InputManager.h"
#include "components/Component.h"
#include "rendering/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

CameraMovementSystem::CameraMovementSystem(EntityAdmin* admin) : m_admin(admin) { }

float velocity = 0.7;
float degrees_per_mouse_unit = 0.02;

const float MAX_VERT_ANGLE = 175.0 * glm::pi<float>() / 180.0;
const float MIN_VERT_ANGLE = 5.0 * glm::pi<float>() / 180.0;

/* Probably want to split this into a player movement system that responds to input and
   doesn't need to know about the camera, and then a camera render system that uses
   the camera + transform to update the Renderer state
   - maybe better yet, a movement system with acceleration/velocity vectors and a player
     input system that simply sets an indicator to start accelerating?
     -> something like an impulse component?
   - later there will probably be separate render systems for the world chunks and entities
     that pass vertex data into the renderer
*/
void CameraMovementSystem::tick()
{
    auto input = m_admin->getInputManager();
    auto renderer = m_admin->getRenderer();
    // Don't know if it actually makes sense for there can be multiple of these yet
    for (int id :
         m_admin->componentView<CameraComponent, TransformComponent, PlayerControlComponent>()) {
        auto cam = m_admin->getComponent<CameraComponent>(id);
        auto transform = m_admin->getComponent<TransformComponent>(id);

        glm::vec3 pos = transform->m_position;
        glm::vec3 up = transform->m_up;
        glm::vec3 right = transform->m_right;

        if (input->m_mouse_delta_x) {
            float angle
                = -1 * input->m_mouse_delta_x * degrees_per_mouse_unit * glm::pi<float>() / 180;
            transform->m_forward = glm::normalize(glm::rotate(transform->m_forward, angle, up));
            transform->m_right = glm::normalize(glm::cross(transform->m_up, transform->m_forward));
        }
        if (input->m_mouse_delta_y) {
            float curr_angle = glm::angle(transform->m_forward, transform->m_up);
            float angle = input->m_mouse_delta_y * degrees_per_mouse_unit * glm::pi<float>() / 180;
            if (angle + curr_angle > MAX_VERT_ANGLE) {
                angle = MAX_VERT_ANGLE - curr_angle;
            } else if (angle + curr_angle < MIN_VERT_ANGLE) {
                angle = curr_angle - MIN_VERT_ANGLE;
            }
            transform->m_forward = glm::normalize(glm::rotate(transform->m_forward, angle, right));
        }

        glm::vec3 forward = transform->m_forward;

        // Right now this is a fixed amount per frame. Will want some kind of time based
        // system later but this works for now
        if (input->isPressed(UP)) {
            transform->m_position = pos + velocity * up;
        }
        if (input->isPressed(DOWN)) {
            transform->m_position = pos - velocity * up;
        }
        // These movements are all backwards compared to what I expect, not sure why yet
        if (input->isPressed(RIGHT)) {
            transform->m_position = pos - velocity * right;
        }
        if (input->isPressed(LEFT)) {
            transform->m_position = pos + velocity * right;
        }
        if (input->isPressed(FORWARD)) {
            transform->m_position = pos + glm::cross(right, up) * velocity;
        }
        if (input->isPressed(BACK)) {
            transform->m_position = pos + glm::cross(up, right) * velocity;
        }

        glm::mat4 view = glm::lookAt(
            transform->m_position, transform->m_position + transform->m_forward, transform->m_up);
        renderer->setViewMatrix(view);
    }
}
