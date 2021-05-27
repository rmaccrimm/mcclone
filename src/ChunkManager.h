#ifndef _CHUNKMANAGER_H_
#define _CHUNKMANAGER_H_

#include <glm/vec3.hpp>
#include <unordered_map>
#include <vector>

#include "RenderObject.h"
#include "Renderer.h"
#include "WorldChunk.h"

class ChunkManager {
public:
    ChunkManager(Renderer* renderer);
    ~ChunkManager();

    std::unordered_map<int, WorldChunk> m_chunks;

    bool checkNeighbour(glm::vec3 position, glm::vec3 direction);

    WorldChunk& getCentralChunk();

    void reloadChunks(glm::vec3 world_center);

private:
    Renderer* m_renderer;
    int m_central_index;
};

#endif /* _CHUNKMANAGER_H_ */
