#include "WorldChunk.h"

#include <glm/vec2.hpp>
#include <iostream>

WorldChunk::WorldChunk() : m_blocks() {
    m_position.x = 0;
    m_position.z = 0;

    for (int x = 0; x < 32; x++) {
	for (int z = 0; z < 32; z++) {
	    for (int y = 0; y < 10; y++) {
		m_blocks[x][y][z] = 1;
	    }
	} 
    }
}
