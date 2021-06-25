#include "PlayerControlSystem.h"
#include "EntityAdmin.h"
#include "InputManager.h"
#include "components/Component.h"
#include "util/math.h"

#include <glm/gtc/constants.hpp>
#include <plog/Log.h>

void PlayerControlSystem::tick()
{
    auto input = m_admin.getInputManager();
    for (int id :
         m_admin.componentView<MovementComponent, TransformComponent, PlayerControlComponent>()) {

        // Not using physics-based rotation, so directly update the transform here
        auto transform = m_admin.getComponent<TransformComponent>(id);
        glm::vec3 rotation(0, 0, 0);

        // Only allow horizontal turns (rotation about y-axis) for player entity
        if (input->m_mouse_delta_x) {
            const float rads = InputManager::DEGREES_PER_MOUSE_UNIT * glm::pi<float>() / 180.0f;
            rotation.y = -1 * input->m_mouse_delta_x * rads;
        }
        rotateEuler(rotation, transform);

        // For translation, set the move component up so physics can use it automatically
        auto move = m_admin.getComponent<MovementComponent>(id);
        glm::vec3& direction = move->direction;

        glm::vec3 frw = transform->m_forward;
        glm::vec3 up = transform->m_up;
        glm::vec3 right = transform->m_right;

        direction
            = (float)((int)input->isPressed(Key::LEFT) - (int)input->isPressed(Key::RIGHT)) * right
            + (float)((int)input->isPressed(Key::UP) - (int)input->isPressed(Key::DOWN)) * up
            + (float)((int)input->isPressed(Key::FORWARD) - (int)input->isPressed(Key::BACK)) * frw;
    }
}
