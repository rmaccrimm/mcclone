#include "CameraControlSystem.h"
#include "EntityAdmin.h"
#include "InputManager.h"
#include "components/Component.h"
#include "util/math.h"

#include <glm/gtc/constants.hpp>
#include <plog/Log.h>


void CameraControlSystem::tick()
{
    auto input = m_admin.getInputManager();
    for (int id :
         m_admin.componentView<TransformComponent, CameraControlComponent>()) {
        auto transform = m_admin.getComponent<TransformComponent>(id);

        const float rads = InputManager::DEGREES_PER_MOUSE_UNIT * glm::pi<float>() / 180.0f;
        glm::vec3 rotation(0, 0, 0);

        if (input->m_mouse_delta_x) {
            rotation.y = -1 * input->m_mouse_delta_x * rads;
        }
        if (input->m_mouse_delta_y) {
            rotation.x = input->m_mouse_delta_y * rads;
        }
        rotateEuler(rotation, transform);
    }
}
