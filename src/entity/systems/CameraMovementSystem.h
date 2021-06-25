#ifndef _CAMERAMOVEMENTSYSTEM_H_
#define _CAMERAMOVEMENTSYSTEM_H_

#include "System.h"

class CameraMovementSystem : public System {
public:
    CameraMovementSystem(EntityAdmin& admin) : System(admin) { }
    void tick() override;
};

#endif /* _CAMERAMOVEMENTSYSTEM_H_ */
