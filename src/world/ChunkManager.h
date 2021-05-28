#ifndef _CHUNKMANAGER_H_
#define _CHUNKMANAGER_H_

#include <glm/vec3.hpp>
#include <unordered_map>
#include <vector>
#include <memory>

#include "RenderObject.h"
#include "Renderer.h"
#include "WorldChunk.h"

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;  
    }
};

class ChunkManager {
public:
    ChunkManager(Renderer* renderer);

    std::unordered_map<std::pair<int, int>, std::unique_ptr<WorldChunk>, pair_hash> m_chunks;

    bool checkNeighbour(glm::vec3 position, glm::vec3 direction);

    void reloadChunks(glm::vec3 world_center);

    std::unique_ptr<WorldChunk>& getCenterChunk();

private:
    Renderer* m_renderer;
    std::pair<int, int> m_central_index;
};

#endif /* _CHUNKMANAGER_H_ */
