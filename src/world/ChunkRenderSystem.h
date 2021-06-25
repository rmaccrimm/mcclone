#ifndef _CHUNKRENDERSYSTEM_H_
#define _CHUNKRENDERSYSTEM_H_

#include "RenderObject.h"
#include <glm/vec3.hpp>

#include "System.h"

/* Should eventually inherit from a System base class
 */
class ChunkRenderSystem : public System {
public:
    ChunkRenderSystem(EntityAdmin& admin) : System(admin) { }
    void tick();

private:
    void loadCubeFaces(glm::vec3 cube_position, RenderObject& target);
};

#endif /* _RENDERSYSTEM_H_ */
