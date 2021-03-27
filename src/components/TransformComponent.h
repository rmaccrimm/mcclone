#ifndef _TRANSFORMCOMPONENT_H_
#define _TRANSFORMCOMPONENT_H_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>


struct TransformComponent
{
    static const int TAG = 1 << __COUNTER__;
    glm::vec3 m_up = glm::vec3(0.0, 1.0, 0.0);
    glm::vec3 m_right = glm::vec3(1.0, 0.0, 0.0);
    glm::vec3 m_forward = glm::vec3(0.0, 0.0, -1.0);
    glm::vec3 m_position = glm::vec3(0.0, 0.0, 0.0);
};

#endif /* _TRANSFORMCOMPONENT_H_ */
