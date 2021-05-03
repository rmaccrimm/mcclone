#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <plog/Log.h>

#include "ChunkManager.h"
#include "EntityAdmin.h"
#include "RenderSystem.h"
#include "Renderer.h"
#include "Vertex.h"
#include "WorldChunk.h"
#include "components/Component.h"

enum CUBE_FACE { LEFT = 0, RIGHT = 1, BOTTOM = 2, TOP = 3, FRONT = 4, BACK = 5 };

// clang-format off
const glm::vec3 FACE_NORMALS[6]  = {
    // LEFT
    glm::vec3(-1, 0, 0),
    // RIGHT
    glm::vec3(1, 0, 0),
    // BOTTOM
    glm::vec3(0, -1, 0),
    // TOP
    glm::vec3(0, 1, 0),
    // FRONT
    glm::vec3(0, 0, -1),
    // BACK
    glm::vec3(0, 0, 1)
};

// TODO store/use the elements in a constant array as well
const glm::vec3 CUBE_FACES[6][4] = {
    {   // LEFT
	glm::vec3(0, 0, 0),                             
	glm::vec3(0, 0, 1),                             
	glm::vec3(0, 1, 0),                             
	glm::vec3(0, 1, 1),                             
    },
    {   // RIGHT
	glm::vec3(1, 0, 1),
	glm::vec3(1, 0, 0),
	glm::vec3(1, 1, 1),
	glm::vec3(1, 1, 0),
    },
    {   // BOTTOM
	glm::vec3(0, 0, 0),
	glm::vec3(1, 0, 0),
	glm::vec3(0, 0, 1),
	glm::vec3(1, 0, 1),
    },
    {   // TOP
	glm::vec3(1, 1, 0),
	glm::vec3(0, 1, 0),
	glm::vec3(1, 1, 1),
	glm::vec3(0, 1, 1),
    },
    {   // FRONT
	glm::vec3(1, 0, 0),
	glm::vec3(0, 0, 0),
	glm::vec3(1, 1, 0),
	glm::vec3(0, 1, 0),
    },
    {   // BACK
	glm::vec3(0, 0, 1),
	glm::vec3(1, 0, 1),
	glm::vec3(0, 1, 1),
	glm::vec3(1, 1, 1) 
    }
};

const glm::vec2 TEX_COORDS[4] = {
    glm::vec2(0, 0),
    glm::vec2(0, 1),
    glm::vec2(1, 0),
    glm::vec2(1, 1)
};
// clang-format on

RenderSystem::RenderSystem(EntityAdmin* admin) : m_admin { admin } { }

const glm::vec3 colors[2] = { glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.3, 0.3, 0.3) };

void RenderSystem::tick()
{
    LOG_INFO << "Transfering data to GPU";
    auto chunk_mgr = m_admin->getChunkManager();
    auto renderer = m_admin->getRenderer();

    std::array<int, 6> indices;
    std::array<Vertex, 4> face;

    for (auto const& chunk : chunk_mgr->m_chunks) {
        for (int y = 0; y < WorldChunk::SPAN_Y; y++) {
            for (int x = 0; x < WorldChunk::SPAN_X; x++) {
                for (int z = 0; z < WorldChunk::SPAN_Z; z++) {
                    if (chunk->m_blocks[x][y][z]) {
                        glm::vec3 chunk_position = glm::vec3(x, y, z);

                        glm::vec3 world_position = chunk_position
                            + glm::vec3(chunk->m_position.x, 0, chunk->m_position.z);
                        glm::mat4 translate = glm::translate(glm::mat4(1), world_position);

                        for (int q = 0; q < 6; q++) {
                            if (!chunk_mgr->checkNeighbour(world_position, FACE_NORMALS[q])) {
                                indices = { 0, 3, 1, 0, 2, 3 };
                                for (int j = 0; j < 4; j++) {
                                    face[j].position
                                        = glm::vec3(translate * glm::vec4(CUBE_FACES[q][j], 1));
				    face[j].texcoords = TEX_COORDS[j];
                                    face[j].normal = FACE_NORMALS[q];
                                }
                                renderer->copyVertexData(face, indices);
                            }
                        }
                    }
                }
            }
        }
    }
}
