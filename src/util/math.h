#ifndef _MATH_H_
#define _MATH_H_

#include <components/TransformComponent.h>

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

const float MAX_VERT_ANGLE = 175.0 * glm::pi<float>() / 180.0;
const float MIN_VERT_ANGLE = 5.0 * glm::pi<float>() / 180.0;

// TODO Doesn't include rotations about z axis currently
// TODO should just return a new transform
inline void rotateEuler(glm::vec3 rotation, TransformComponent& transform)
{
    glm::vec3 up = transform.m_up;
    glm::vec3 right = transform.m_right;

    transform.m_forward = glm::normalize(glm::rotate(transform.m_forward, rotation.y, up));
    transform.m_right = glm::normalize(glm::cross(transform.m_up, transform.m_forward));

    float curr_angle = glm::angle(transform.m_forward, transform.m_up);
    float angle = rotation.x;
    if (angle + curr_angle > MAX_VERT_ANGLE) {
	angle = MAX_VERT_ANGLE - curr_angle;
    } else if (angle + curr_angle < MIN_VERT_ANGLE) {
	angle = curr_angle - MIN_VERT_ANGLE;
    }
    transform.m_forward = glm::normalize(glm::rotate(transform.m_forward, angle, right));
}

#endif /* _MATH_H_ */
