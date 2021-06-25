#ifndef _MOVEMENTSYSTEM_H_
#define _MOVEMENTSYSTEM_H_

#include "System.h"

class MovementSystem : public System {
public:
    MovementSystem(EntityAdmin& admin) : System(admin) { }
    void tick() override;
};

#endif /* _MOVEMENTSYSTEM_H_ */
