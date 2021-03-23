#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "RenderSystem.h"
#include "Renderer.h"
#include "EntityAdmin.h"
#include "components/Component.h"
#include "ChunkManager.h"
#include "WorldChunk.h"


enum CUBE_FACE {
    LEFT = 0,
    RIGHT = 1 ,
    BOTTOM = 2,
    TOP = 3,
    FRONT = 4,
    BACK = 5
};

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

bool checkNeighbour(const WorldChunk *chunk, glm::vec3 position, int face) {
    auto n = position + FACE_NORMALS[face];
    return (n.x >= 0 && n.x < WorldChunk::SPAN_X)
	&& (n.y >= 0 && n.y < WorldChunk::SPAN_Y)
	&& (n.z >= 0 && n.z < WorldChunk::SPAN_Z)
	&& chunk->m_blocks[(int)n.x][(int)n.y][(int)n.z];
}

RenderSystem::RenderSystem(EntityAdmin *admin) : m_admin{admin} {}

void RenderSystem::tick()
{
    int i = 0;
    std::vector<glm::vec3> face(4, glm::vec3());
    std::vector<int> indices(6, 0);
    auto renderer = m_admin->getRenderer();
    
    for (auto const &chunk: m_admin->getChunkManager()->m_chunks) {
	for (int y = 0; y < WorldChunk::SPAN_Y; y++) {
	    for (int x = 0; x < WorldChunk::SPAN_X; x++) {
		for (int z = 0; z < WorldChunk::SPAN_Z; z++) {
		    
		    if (chunk->m_blocks[x][y][z]) {
			glm::vec3 position = glm::vec3(x, y, z);
			glm::mat4 translate = glm::translate(glm::mat4(1), glm::vec3(x, y, z));
			
			for (int q = 0; q < 6; q++) {
			    if (!checkNeighbour(chunk, position, q)) {
				for (int j = 0; j < 4; j++) {
				    face[j] = translate * glm::vec4(CUBE_FACES[q][j], 1);
                                }
                                indices = {i/3, i/3 + 3, i/3 + 1, i/3, i/3 + 2, i/3 + 3};
				renderer->appendVertices(face.begin(), face.end());
				renderer->appendIndices(indices.begin(), indices.end());
			    }
                        }
                    }
                }
            }
        }
    }
}
