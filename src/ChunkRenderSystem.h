#ifndef _CHUNKRENDERSYSTEM_H_
#define _CHUNKRENDERSYSTEM_H_

#include <glm/vec3.hpp>
#include "RenderObject.h"

class EntityAdmin;

/* Should eventually inherit from a System base class
 */
class ChunkRenderSystem
{
public:
    ChunkRenderSystem(EntityAdmin* admin);
    void tick();
private:
    void loadCubeFaces(glm::vec3 cube_position, RenderObject &target);
    EntityAdmin* m_admin;
};

#endif /* _RENDERSYSTEM_H_ */
