#ifndef _CAMERACOMPONENT_H_
#define _CAMERACOMPONENT_H_

#include <glm/mat4x4.hpp>
#include "TransformComponent.h"

struct CameraComponent {
    static const int TAG = 1 << __COUNTER__;
    glm::mat4 m_projection = glm::proj
};



#endif /* _CAMERACOMPONENT_H_ */
