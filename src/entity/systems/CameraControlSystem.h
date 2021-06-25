#ifndef _CAMERACONTROLSYSTEM_H_
#define _CAMERACONTROLSYSTEM_H_

#include "System.h"

class CameraControlSystem : public System {
public:
    CameraControlSystem(EntityAdmin& admin) : System(admin) { }
    void tick() override;
};

#endif /* _CAMERACONTROLSYSTEM_H_ */
