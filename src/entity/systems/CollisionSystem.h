#ifndef _COLLISIONSYSTEM_H_
#define _COLLISIONSYSTEM_H_

#include "System.h"

class CollisionSystem : public System {
public:
    CollisionSystem(EntityAdmin& admin) : System(admin) { }
    void tick() override;
};

#endif /* _COLLISIONSYSTEM_H_ */
