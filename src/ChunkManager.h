#ifndef _CHUNKMANAGER_H_
#define _CHUNKMANAGER_H_

#include <vector>
#include <glm/vec3.hpp>

class WorldChunk;

class ChunkManager
{
public:
    ChunkManager();
    ~ChunkManager();
    std::vector<WorldChunk*> m_chunks;

    bool checkNeighbour(glm::vec3 position, glm::vec3 direction);
};


#endif /* _CHUNKMANAGER_H_ */
