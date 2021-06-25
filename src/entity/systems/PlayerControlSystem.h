#ifndef _PLAYERCONTROLSYSTEM_H_
#define _PLAYERCONTROLSYSTEM_H_

#include "System.h"

class PlayerControlSystem : public System {
public:
    PlayerControlSystem(EntityAdmin& admin) : System(admin) { }
    void tick() override;
};

#endif /* _PLAYERCONTROLSYSTEM_H_ */
