#ifndef _TRANSFORMCOMPONENT_H_
#define _TRANSFORMCOMPONENT_H_

#include <glm/mat4x4.hpp>

struct TransformComponent
{
    static const int TAG = 1 << __COUNTER__;
    glm::mat4x4 m_matrix;
};

#endif /* _TRANSFORMCOMPONENT_H_ */
