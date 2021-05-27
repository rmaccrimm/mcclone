#include "ChunkManager.h"
#include "WorldChunk.h"
#include <plog/Log.h>

const int WORLD_SIZE = 3;

int map_key(int x, int z) { return WORLD_SIZE * x + z; }

ChunkManager::ChunkManager(Renderer* renderer) : m_renderer { renderer }
{
    for (int i = 0; i < WORLD_SIZE * WORLD_SIZE; i++) {
        int x = i / WORLD_SIZE;
        int z = i % WORLD_SIZE;
        WorldChunk chunk(x * 32, z * 32, m_renderer->newRenderObject());
        m_chunks.insert({ map_key(chunk.m_origin.x, chunk.m_origin.z), chunk });
        m_central_index = map_key(0, 0);
    }
}

ChunkManager::~ChunkManager() { }

bool ChunkManager::checkNeighbour(glm::vec3 position, glm::vec3 direction)
{
    glm::vec3 cube_coord = position + direction;
    
    int internal_coord_x = static_cast<int>(cube_coord.x) % 32;
    int internal_coord_y = static_cast<int>(cube_coord.y);
    int internal_coord_z = static_cast<int>(cube_coord.z) % 32;

    int chunk_coord_x = static_cast<int>(cube_coord.x) / 32 * 32;
    int chunk_coord_z = static_cast<int>(cube_coord.z) / 32 * 32;

    auto chunk_iter = m_chunks.find(map_key(chunk_coord_x, chunk_coord_z));
    if (chunk_iter == m_chunks.end()) {
        return false;
    }
    auto& chunk = chunk_iter->second;

    return (cube_coord.x >= 0 && cube_coord.x < WORLD_SIZE * WorldChunk::SPAN_X)
        && (cube_coord.z >= 0 && cube_coord.z < WORLD_SIZE * WorldChunk::SPAN_Z)
        && (cube_coord.y >= 0 && cube_coord.y < WorldChunk::SPAN_Y)
        && chunk.m_blocks[internal_coord_x][internal_coord_y][internal_coord_z];
}

WorldChunk& ChunkManager::getCentralChunk() { return m_chunks.find(m_central_index)->second; }

void reloadChunks(glm::vec3 world_center)
{
    glm::vec3 central_chunk_coord = world_center / 32.0f;
}
