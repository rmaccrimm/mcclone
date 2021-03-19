#include "WorldChunk.h"

#include <glm/vec2.hpp>
#include <iostream>

WorldChunk::WorldChunk() : m_blocks() {
    std::cout << "Initializing chunk..." << std::endl;
    m_position = glm::vec2(0, 0);
    m_num_blocks = 32 * 32;

    for (int x = 0; x < 32; x++) {
	for (int z = 0; z < 32; z++) {
	    m_blocks[x][0][z] = 1;
	} 
    }
}
