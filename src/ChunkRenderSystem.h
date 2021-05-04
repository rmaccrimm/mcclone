#ifndef _CHUNKRENDERSYSTEM_H_
#define _CHUNKRENDERSYSTEM_H_

class EntityAdmin;

/* Should eventually inherit from a System base class
 */
class ChunkRenderSystem
{
public:
    ChunkRenderSystem(EntityAdmin* admin);
    void tick();
private:
    EntityAdmin* m_admin;
};

#endif /* _RENDERSYSTEM_H_ */
