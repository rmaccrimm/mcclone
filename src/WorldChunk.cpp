#include "WorldChunk.h"

#include <glm/vec2.hpp>
#include <iostream>
#include <stdio.h>
#include <math.h>

#include "yocto_math.h"
#include "yocto_noise.h"





WorldChunk::WorldChunk(int _x, int _z) : m_blocks() {
    m_position.x = _x;
    m_position.z = _z;

    for (int x = 0; x < 32; x++) {
	for (int z = 0; z < 32; z++) {
	    auto coord = yocto::vec2f {
		(float)(m_position.x + x) * 1.0f / 64.0f,
		(float)(m_position.z + z) * 1.0f / 64.0f };
	    int height = yocto::perlin_noise(coord) * 30 + 10;
	    for (int y = 0; y < height; y++) {
		m_blocks[x][y][z] = y == (height - 1) ? 1 : 2;
	    }
	} 
    }
}
