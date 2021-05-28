
#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <map>
#include <plog/Log.h>

#include "ChunkManager.h"
#include "ChunkRenderSystem.h"
#include "EntityAdmin.h"
#include "RenderObject.h"
#include "Renderer.h"
#include "Vertex.h"
#include "WorldChunk.h"
#include "components/Component.h"

const std::vector<Vertex> CUBE = {
    // 0
    { .position = { 0.000000, 1.000000, 0.000000 },
      .normal = { 0.000000, 1.000000, 0.000000 },
      .texcoords = { 0.333333, 0.833333 } },
    // 1
    { .position = { 1.000000, 1.000000, 1.000000 },
      .normal = { 0.000000, 1.000000, 0.000000 },
      .texcoords = { 0.166667, 1.000000 } },
    // 2
    { .position = { 1.000000, 1.000000, 0.000000 },
      .normal = { 0.000000, 1.000000, 0.000000 },
      .texcoords = { 0.166667, 0.833333 } },
    // 3
    { .position = { 1.000000, 1.000000, 1.000000 },
      .normal = { 0.000000, 0.000000, 1.000000 },
      .texcoords = { 0.500000, 1.000000 } },
    // 4
    { .position = { 0.000000, 0.000000, 1.000000 },
      .normal = { 0.000000, 0.000000, 1.000000 },
      .texcoords = { 0.333333, 0.833333 } },
    // 5
    { .position = { 1.000000, 0.000000, 1.000000 },
      .normal = { 0.000000, 0.000000, 1.000000 },
      .texcoords = { 0.500000, 0.833333 } },
    // 6
    { .position = { 0.000000, 1.000000, 1.000000 },
      .normal = { -1.000000, 0.000000, 0.000000 },
      .texcoords = { 1.000000, 1.000000 } },
    // 7
    { .position = { 0.000000, 0.000000, 0.000000 },
      .normal = { -1.000000, 0.000000, 0.000000 },
      .texcoords = { 0.833333, 0.833333 } },
    // 8
    { .position = { 0.000000, 0.000000, 1.000000 },
      .normal = { -1.000000, 0.000000, 0.000000 },
      .texcoords = { 1.000000, 0.833333 } },
    // 9
    { .position = { 1.000000, 0.000000, 0.000000 },
      .normal = { 0.000000, -1.000000, 0.000000 },
      .texcoords = { 0.166667, 1.000000 } },
    // 10
    { .position = { 0.000000, 0.000000, 1.000000 },
      .normal = { 0.000000, -1.000000, 0.000000 },
      .texcoords = { 0.000000, 0.833333 } },
    // 11
    { .position = { 0.000000, 0.000000, 0.000000 },
      .normal = { 0.000000, -1.000000, 0.000000 },
      .texcoords = { 0.166667, 0.833333 } },
    // 12
    { .position = { 1.000000, 1.000000, 0.000000 },
      .normal = { 1.000000, 0.000000, 0.000000 },
      .texcoords = { 0.666667, 1.000000 } },
    // 13
    { .position = { 1.000000, 0.000000, 1.000000 },
      .normal = { 1.000000, 0.000000, 0.000000 },
      .texcoords = { 0.500000, 0.833333 } },
    // 14
    { .position = { 1.000000, 0.000000, 0.000000 },
      .normal = { 1.000000, 0.000000, 0.000000 },
      .texcoords = { 0.666667, 0.833333 } },
    // 15
    { .position = { 0.000000, 1.000000, 0.000000 },
      .normal = { 0.000000, 0.000000, -1.000000 },
      .texcoords = { 0.833333, 1.000000 } },
    // 16
    { .position = { 1.000000, 0.000000, 0.000000 },
      .normal = { 0.000000, 0.000000, -1.000000 },
      .texcoords = { 0.666667, 0.833333 } },
    // 17
    { .position = { 0.000000, 0.000000, 0.000000 },
      .normal = { 0.000000, 0.000000, -1.000000 },
      .texcoords = { 0.833333, 0.833333 } },
    // 18
    { .position = { 0.000000, 1.000000, 1.000000 },
      .normal = { 0.000000, 1.000000, 0.000000 },
      .texcoords = { 0.333333, 1.000000 } },
    // 19
    { .position = { 0.000000, 1.000000, 1.000000 },
      .normal = { 0.000000, 0.000000, 1.000000 },
      .texcoords = { 0.333333, 1.000000 } },
    // 20
    { .position = { 1.000000, 1.000000, 1.000000 },
      .normal = { 1.000000, 0.000000, 0.000000 },
      .texcoords = { 0.500000, 1.000000 } },
    // 21
    { .position = { 0.000000, 1.000000, 0.000000 },
      .normal = { -1.000000, 0.000000, 0.000000 },
      .texcoords = { 0.833333, 1.000000 } },
    // 22
    { .position = { 1.000000, 1.000000, 0.000000 },
      .normal = { 0.000000, 0.000000, -1.000000 },
      .texcoords = { 0.666667, 1.000000 } },
    // 23
    { .position = { 1.000000, 0.000000, 1.000000 },
      .normal = { 0.000000, -1.000000, 0.000000 },
      .texcoords = { 0.000000, 1.000000 } },
};

const std::vector<unsigned int> INDICES = {
    0, 1,  2, 3, 4,  5, 6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
    0, 18, 1, 3, 19, 4, 12, 20, 13, 6, 21, 7,  15, 22, 16, 9,  23, 10,
};

ChunkRenderSystem::ChunkRenderSystem(EntityAdmin* admin) : m_admin { admin } { }

void ChunkRenderSystem::loadCubeFaces(glm::vec3 cube_position, RenderObject& target)
{
    auto chunk_mgr = m_admin->getChunkManager();
    int starting_index = target.vertices.size();
    glm::mat4 translate = glm::translate(glm::mat4(1), cube_position);

    bool vert_used[24] = { 0 };
    std::vector<unsigned int> indices_used;
    for (int i = 0; i < 36; i += 3) {
        auto& v = CUBE[INDICES[i]];
        if (chunk_mgr->checkNeighbour(cube_position, v.normal)) {
            continue;
        }
        for (int j = 0; j < 3; j++) {
            int index = INDICES[i + j];
            indices_used.push_back(index);
            vert_used[index] = true;
        }
    }

    std::map<int, int> index_map;
    int next = 0;
    for (int i = 0; i < 24; i++) {
        if (vert_used[i]) {
            index_map[i] = next++;
            auto v = CUBE[i];
            target.vertices.push_back({ .position = glm::vec3(translate * glm::vec4(v.position, 1)),
                                        .normal = v.normal,
                                        .texcoords = v.texcoords });
        }
    }
    for (auto i : indices_used) {
        target.indices.push_back(starting_index + index_map.at(i));
    }
}

void ChunkRenderSystem::tick()
{
    LOG_INFO << "Transfering data to GPU";
    auto chunk_mgr = m_admin->getChunkManager();
    auto renderer = m_admin->getRenderer();

    // TODO - some kind of chunk iter
    for (auto& [key, chunk] : chunk_mgr->m_chunks) {
        LOG_INFO << "Loading Chunk (" << chunk->origin.x << ", " << chunk->origin.z << ")";
        RenderObject render_obj;
        for (int y = 0; y < WorldChunk::SPAN_Y; y++) {
            for (int x = 0; x < WorldChunk::SPAN_X; x++) {
                for (int z = 0; z < WorldChunk::SPAN_Z; z++) {
                    if (chunk->blocks[x][y][z]) {
                        glm::vec3 chunk_position = glm::vec3(x, y, z);
                        glm::vec3 world_position = chunk_position + chunk->origin;
                        loadCubeFaces(world_position, render_obj);
                    }
                }
            }
        }
        renderer->updateRenderObject(chunk->render_obj_id, render_obj);
    }
}
