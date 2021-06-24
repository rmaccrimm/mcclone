#ifndef _CAMERACOMPONENT_H_
#define _CAMERACOMPONENT_H_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct CameraComponent {
    static const int TAG = 1 << __COUNTER__;
    
    // Unused right now (I think) - idea was to link FOV settings to the camera entity
    glm::mat4 m_projection = glm::mat4(1.0);
    
    // The id of the entity the camera points at
    int target;
};



#endif /* _CAMERACOMPONENT_H_ */
