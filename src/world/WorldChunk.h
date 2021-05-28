#ifndef _WORLDCHUNK_H_
#define _WORLDCHUNK_H_

#include <components/TransformComponent.h>

#include <cstdint>
#include <glm/vec3.hpp>

typedef uint8_t block_id;

class WorldChunk {
public:
    WorldChunk(glm::vec3 origin, unsigned int render_obj_id);
    
    static const int SPAN_X = 32;
    static const int SPAN_Y = 256;
    static const int SPAN_Z = 32;
    glm::vec3 origin;
    block_id blocks[SPAN_X][SPAN_Y][SPAN_Z];
    unsigned int render_obj_id;
};

#endif /* _WORLDCHUNK_H_ */
