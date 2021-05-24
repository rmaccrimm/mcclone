#ifndef _CHUNKMANAGER_H_
#define _CHUNKMANAGER_H_

#include <vector>
#include <glm/vec3.hpp>
#include <map>

#include "RenderObject.h"
#include "Renderer.h"


class WorldChunk;

class ChunkManager
{
public:
    ChunkManager(Renderer* renderer);
    ~ChunkManager();
    std::vector<WorldChunk*> m_chunks;
    // TODO - change this to a map from coords to renderobj id
    std::vector<unsigned int> m_render_objs;

    bool checkNeighbour(glm::vec3 position, glm::vec3 direction);
private:
    Renderer* m_renderer;
};


#endif /* _CHUNKMANAGER_H_ */
