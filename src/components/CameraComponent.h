#ifndef _CAMERACOMPONENT_H_
#define _CAMERACOMPONENT_H_

#include <glm/mat4x4.hpp>

struct CameraComponent {
    static const int TAG = 1 << __COUNTER__;
    glm::mat4 m_projection = glm::mat4(1.0);
};



#endif /* _CAMERACOMPONENT_H_ */
