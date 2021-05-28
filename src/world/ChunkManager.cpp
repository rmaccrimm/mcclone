#include "ChunkManager.h"
#include "WorldChunk.h"
#include <plog/Log.h>

#include "yocto_math.h"
#include "yocto_noise.h"

#include <stack>

const int WORLD_SIZE = 2;

void generateChunk(std::unique_ptr<WorldChunk>& chunk)
{
    for (int x = 0; x < 32; x++) {
        for (int z = 0; z < 32; z++) {
            auto coord = yocto::vec2f { (chunk->origin.x + (float)x) * 1.0f / 64.0f,
                                        (chunk->origin.z + (float)z) * 1.0f / 64.0f };
            int height = static_cast<int>(yocto::perlin_noise(coord) * 30.0f) + 10;
            for (int y = 0; y < height; y++) {
                chunk->blocks[x][y][z] = y == (height - 1) ? 1 : 2;
            }
        }
    }
}

int roundToNegativeInf(int x, int multiple)
{
    return (x - (x < 0 ? multiple - 1 : 0)) / multiple * multiple;
}

int getRemainder(int x, int multiple) { return x - roundToNegativeInf(x, multiple); }

glm::ivec3 chunkCoord(glm::vec3 world_coords)
{
    return glm::ivec3(
        roundToNegativeInf(world_coords.x, WorldChunk::SPAN_X),
        0,
        roundToNegativeInf(world_coords.z, WorldChunk::SPAN_Z));
}

glm::ivec3 internalCoord(glm::vec3 world_coords)
{
    return glm::ivec3(
        getRemainder(world_coords.x, WorldChunk::SPAN_X),
        world_coords.y,
        getRemainder(world_coords.z, WorldChunk::SPAN_Z));
}

ChunkManager::ChunkManager(Renderer* renderer) : m_renderer { renderer } { }

bool ChunkManager::checkNeighbour(glm::vec3 position, glm::vec3 direction)
{
    glm::vec3 block_coord = position + direction;

    auto internal_coord = internalCoord(block_coord);
    auto chunk_coord = chunkCoord(block_coord);

    auto chunk_iter = m_chunks.find({ chunk_coord.x, chunk_coord.z });
    if (chunk_iter == m_chunks.end()) {
        return false;
    }
    auto& chunk = chunk_iter->second;

    return (block_coord.x >= 0 && block_coord.x < WORLD_SIZE * WorldChunk::SPAN_X)
        && (block_coord.z >= 0 && block_coord.z < WORLD_SIZE * WorldChunk::SPAN_Z)
        && (block_coord.y >= 0 && block_coord.y < WorldChunk::SPAN_Y)
        && chunk->blocks[internal_coord.x][internal_coord.y][internal_coord.z];
}

std::unique_ptr<WorldChunk>& ChunkManager::getCenterChunk() { return m_chunks.at(m_central_index); }

void ChunkManager::reloadChunks(glm::vec3 world_center)
{
    auto center_chunk_coords = chunkCoord(world_center);
    LOG_INFO << "Player position is (" << world_center.x << ", " << world_center.y << ", "
             << world_center.z << ")";
    LOG_INFO << "Central chunk is (" << center_chunk_coords.x << ", " << center_chunk_coords.z
             << ")";
    m_central_index = { center_chunk_coords.x, center_chunk_coords.z };

    std::vector<std::pair<int, int>> chunks_to_unload;

    for (auto& [key, chunk_ptr] : m_chunks) {
        int x_dist = abs(chunk_ptr->origin.x - center_chunk_coords.x) / WorldChunk::SPAN_X;
        int z_dist = abs(chunk_ptr->origin.z - center_chunk_coords.z) / WorldChunk::SPAN_Z;

        if (x_dist >= WORLD_SIZE || z_dist >= WORLD_SIZE) {
            chunks_to_unload.push_back(key);
        }
    }

    // Save the render object ids to reuse in the renderer
    std::stack<unsigned int> render_ids;
    for (auto key : chunks_to_unload) {
        auto iter = m_chunks.find(key);
        auto coord = iter->second->origin;
        LOG_DEBUG << "Unloading chunk (" << coord.x << ", " << coord.z << ")";
        render_ids.push(iter->second->render_obj_id);
        m_chunks.erase(iter);
    }

    for (int i = -WORLD_SIZE + 1; i < WORLD_SIZE; i++) {
        for (int j = -WORLD_SIZE + 1; j < WORLD_SIZE; j++) {
            auto chunk_coord = center_chunk_coords
                + glm::ivec3(WorldChunk::SPAN_X * i, 0, WorldChunk::SPAN_Z * j);
            std::pair<int, int> key = { chunk_coord.x, chunk_coord.z };
            if (!m_chunks.contains(key)) {
                LOG_DEBUG << "Loading chunk (" << chunk_coord.x << ", " << chunk_coord.z << ")";
                unsigned int render_id;
                if (render_ids.size() > 0) {
                    render_id = render_ids.top();
                    render_ids.pop();
                } else {
                    render_id = m_renderer->newRenderObject();
                }
                m_chunks.insert({ key, std::make_unique<WorldChunk>(chunk_coord, render_id) });
                generateChunk(m_chunks[key]);
            }
        }
    }
}
