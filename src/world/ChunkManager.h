#ifndef _CHUNKMANAGER_H_
#define _CHUNKMANAGER_H_

#include <glm/vec3.hpp>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "RenderObject.h"
#include "Renderer.h"
#include "WorldChunk.h"

class ChunkManager {
public:
    ChunkManager(Renderer& renderer, int world_size);
    ChunkManager(const ChunkManager&) = delete;
    ChunkManager& operator=(const ChunkManager&) = delete;

    std::vector<std::unique_ptr<WorldChunk>> m_chunks;

    bool checkNeighbour(const glm::vec3& position, const glm::vec3& direction);

    void reloadChunks(glm::vec3 world_center);

    std::unique_ptr<WorldChunk>& getCenterChunk();

private:
    std::optional<int> getChunkIndex(glm::ivec3& chunk_origin, glm::ivec3& center_chunk_origin);

    Renderer& m_renderer;
    unsigned int m_central_index;
    int m_world_size;
};

#endif /* _CHUNKMANAGER_H_ */
