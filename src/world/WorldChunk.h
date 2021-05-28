#ifndef _WORLDCHUNK_H_
#define _WORLDCHUNK_H_

#include <components/TransformComponent.h>

#include <cstdint>
#include <glm/vec3.hpp>

typedef uint8_t block_id;

class WorldChunk {
public:
    WorldChunk(glm::ivec3 origin, unsigned int render_obj_id);
    
    static const int SPAN_X = 32;
    static const int SPAN_Y = 256;
    static const int SPAN_Z = 32;
    
    // World-space coordinates for chunk. The chunk fills origin.x <= x < origin.x + SPAN_X and
    // likewise in the z direction (y runs from 0 to SPAN_Y)
    glm::ivec3 origin;

    // Indicates that the chunk should be re-rendered
    bool updated;

    // A reference to the internal rendering data in the Renderer
    unsigned int render_obj_id;

    // Block data
    block_id blocks[SPAN_X][SPAN_Y][SPAN_Z];
};

#endif /* _WORLDCHUNK_H_ */
