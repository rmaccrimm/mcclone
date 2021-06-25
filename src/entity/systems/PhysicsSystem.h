#ifndef _PHYSICSSYSTEM_H_
#define _PHYSICSSYSTEM_H_

#include "System.h"

class PhysicsSystem : public System {
public:
    PhysicsSystem(EntityAdmin& admin) : System(admin) { }
    void tick() override;
};

#endif /* _PHYSICSSYSTEM_H_ */
