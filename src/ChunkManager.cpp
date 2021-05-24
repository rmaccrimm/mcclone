#include "ChunkManager.h"
#include "WorldChunk.h"
#include <plog/Log.h>

const int WORLD_SIZE = 10;

ChunkManager::ChunkManager(Renderer* renderer): m_renderer{renderer}
{
    for (int i = 0; i < WORLD_SIZE * WORLD_SIZE; i++) {
        int x = i / WORLD_SIZE;
        int z = i % WORLD_SIZE;
        WorldChunk* chunk = new WorldChunk(x * 32, z * 32, m_renderer->newRenderObject());
        chunk->m_position.x = 32 * x;
        chunk->m_position.z = 32 * z;
        m_chunks.push_back(chunk);
    }
}

ChunkManager::~ChunkManager()
{
    for (int i = 0; i < WORLD_SIZE * WORLD_SIZE; i++) {
        delete m_chunks[i];
    }
}

bool ChunkManager::checkNeighbour(glm::vec3 position, glm::vec3 direction)
{
    glm::vec3 cube_coord = position + direction;
    glm::vec3 chunk_coord = cube_coord / 32.0f;
    glm::vec3 internal_coord
        = glm::vec3((int)cube_coord.x % 32, cube_coord.y, (int)cube_coord.z % 32);
    int chunk_index = WORLD_SIZE * (int)chunk_coord.x + (int)chunk_coord.z;
    if (chunk_index < 0 || chunk_index >= WORLD_SIZE * WORLD_SIZE) {
	return false;
    }
    auto const &chunk = m_chunks[chunk_index];

    return (cube_coord.x >= 0 && cube_coord.x < WORLD_SIZE * WorldChunk::SPAN_X)
        && (cube_coord.z >= 0 && cube_coord.z < WORLD_SIZE * WorldChunk::SPAN_Z)
        && (cube_coord.y >= 0 && cube_coord.y < WorldChunk::SPAN_Y)
        && chunk->m_blocks[(int)internal_coord.x][(int)internal_coord.y][(int)internal_coord.z];
}
