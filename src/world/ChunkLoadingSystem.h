#ifndef _CHUNKLOADINGSYSTEM_H_
#define _CHUNKLOADINGSYSTEM_H_

#include "System.h"

class ChunkLoadingSystem : public System {
public:
    ChunkLoadingSystem(EntityAdmin& admin) : System(admin) { }
    void tick();
};

#endif /* _CHUNKLOADINGSYSTEM_H_ */
