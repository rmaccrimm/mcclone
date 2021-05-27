#include "WorldChunk.h"

#include <glm/vec2.hpp>
#include <iostream>
#include <math.h>
#include <stdio.h>

#include "yocto_math.h"
#include "yocto_noise.h"

WorldChunk::WorldChunk(int x, int z, unsigned int render_obj_id) :
    m_blocks(), m_render_obj_id { render_obj_id }
{
    m_origin = { static_cast<float>(x), 0.0f, static_cast<float>(z) };

    for (int x = 0; x < 32; x++) {
        for (int z = 0; z < 32; z++) {
            auto coord = yocto::vec2f { (float)(m_origin.x + x) * 1.0f / 64.0f,
                                        (float)(m_origin.z + z) * 1.0f / 64.0f };
            int height = yocto::perlin_noise(coord) * 30 + 10;
            for (int y = 0; y < height; y++) {
                m_blocks[x][y][z] = y == (height - 1) ? 1 : 2;
            }
        }
    }
}
