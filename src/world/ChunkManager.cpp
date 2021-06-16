#include "ChunkManager.h"
#include "WorldChunk.h"
#include "utils.h"
#include "yocto_math.h"
#include "yocto_noise.h"
#include <cmath>
#include <plog/Log.h>

#include <stack>

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

inline std::optional<int>
ChunkManager::getChunkIndex(glm::ivec3& chunk_origin, glm::ivec3& center_chunk_origin)
{
    int rel_x = (chunk_origin.x - center_chunk_origin.x) / WorldChunk::SPAN_X;
    int rel_z = (chunk_origin.z - center_chunk_origin.z) / WorldChunk::SPAN_Z;
    if (abs(rel_x) > WORLD_SIZE || abs(rel_z) > WORLD_SIZE) {
        return {};
    }
    int offset_x = rel_x + WORLD_SIZE;
    int offset_z = rel_z + WORLD_SIZE;
    return (2 * WORLD_SIZE + 1) * offset_x + offset_z;
}

ChunkManager::ChunkManager(Renderer* renderer) : m_renderer { renderer }, m_chunks { nullptr }
{
    m_central_index = (2 * WORLD_SIZE + 1) * WORLD_SIZE + WORLD_SIZE;
    glm::ivec3 center_chunk_coords(0, 0, 0);

    for (int i = -WORLD_SIZE; i <= WORLD_SIZE; i++) {
        for (int j = -WORLD_SIZE; j <= WORLD_SIZE; j++) {
            auto chunk_coord = center_chunk_coords
                + glm::ivec3(WorldChunk::SPAN_X * i, 0, WorldChunk::SPAN_Z * j);

            auto new_index = getChunkIndex(chunk_coord, center_chunk_coords).value();
            LOG_DEBUG << "Loading chunk (" << chunk_coord.x << ", " << chunk_coord.z << ")";
            auto render_id = m_renderer->newRenderObject();
            m_chunks[new_index] = std::make_unique<WorldChunk>(chunk_coord, render_id);
            generateChunk(m_chunks[new_index]);
        }
    }
}

bool ChunkManager::checkNeighbour(const glm::vec3& position, const glm::vec3& direction)
{
    glm::vec3 block_coord = position + direction;

    auto internal_coord = internalCoord(block_coord);
    auto chunk_coord = chunkCoord(block_coord);
    auto opt_chunk_index = getChunkIndex(chunk_coord, m_chunks[m_central_index]->origin);
    // don't want to render a wall at the edge of the loaded chunks, so treat space beyond
    // horizontal limit as filled
    if (!opt_chunk_index.has_value()) {
        return true;
    }
    // We do want to draw faces at the top/bottom of the world limit, so treat space beyond
    // vertical limit as empty
    if (internal_coord.y < 0 || internal_coord.y >= WorldChunk::SPAN_Y) {
        return false;
    }

    auto& chunk = m_chunks[opt_chunk_index.value()];
    return chunk->blocks[internal_coord.x][internal_coord.y][internal_coord.z];
}

std::unique_ptr<WorldChunk>& ChunkManager::getCenterChunk() { return m_chunks[m_central_index]; }

void ChunkManager::reloadChunks(glm::vec3 world_center)
{
    auto center_chunk_coords = chunkCoord(world_center);
    LOG_VERBOSE << "Player position is (" << world_center.x << ", " << world_center.y << ", "
                << world_center.z << ")";
    LOG_VERBOSE << "Central chunk is (" << center_chunk_coords.x << ", " << center_chunk_coords.z
                << ")";

    auto old_center = m_chunks[m_central_index]->origin;
    std::vector<int> chunks_to_unload;

    for (auto& chunk_ptr : m_chunks) {
        int x_dist = abs(chunk_ptr->origin.x - center_chunk_coords.x) / WorldChunk::SPAN_X;
        int z_dist = abs(chunk_ptr->origin.z - center_chunk_coords.z) / WorldChunk::SPAN_Z;

        if (chunk_ptr && (x_dist > WORLD_SIZE || z_dist > WORLD_SIZE)) {
            auto index = getChunkIndex(chunk_ptr->origin, old_center);
            chunks_to_unload.push_back(index.value());
        }
    }

    // Save the render object ids to reuse in the renderer
    std::stack<unsigned int> render_ids;
    for (auto ind : chunks_to_unload) {
        auto coord = m_chunks[ind]->origin;
        LOG_VERBOSE << "Unloading chunk (" << coord.x << ", " << coord.z << ")";
        render_ids.push(m_chunks[ind]->render_obj_id);
        m_chunks[ind].reset();
    }

    const int len = 2 * WORLD_SIZE + 1;
    std::unique_ptr<WorldChunk> tmp_grid[len * len];

    for (int i = 0; i < len * len; i++) {
        tmp_grid[i].swap(m_chunks[i]);
    }

    for (int i = -WORLD_SIZE; i <= WORLD_SIZE; i++) {
        for (int j = -WORLD_SIZE; j <= WORLD_SIZE; j++) {
            auto chunk_coord = center_chunk_coords
                + glm::ivec3(WorldChunk::SPAN_X * i, 0, WorldChunk::SPAN_Z * j);

            auto old_index = getChunkIndex(chunk_coord, old_center);
            auto new_index = getChunkIndex(chunk_coord, center_chunk_coords).value();

            if (old_index.has_value()) {
                m_chunks[new_index].swap(tmp_grid[old_index.value()]);
            } else {
                LOG_VERBOSE << "Loading chunk (" << chunk_coord.x << ", " << chunk_coord.z << ")";
                unsigned int render_id;
                if (render_ids.size() > 0) {
                    render_id = render_ids.top();
                    render_ids.pop();
                } else {
                    render_id = m_renderer->newRenderObject();
                }
                m_chunks[new_index] = std::make_unique<WorldChunk>(chunk_coord, render_id);
                generateChunk(m_chunks[new_index]);
            }
        }
    }
}
