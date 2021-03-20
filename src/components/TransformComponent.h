#ifndef _TRANSFORMCOMPONENT_H_
#define _TRANSFORMCOMPONENT_H_

#include <glm/mat4x4.hpp>

struct TransformComponent
{
    static const int TAG = 1 << __COUNTER__;
    glm::mat4 m_matrix = glm::mat4(1.0);
};

#endif /* _TRANSFORMCOMPONENT_H_ */
