#ifndef _WORLDCHUNK_H_
#define _WORLDCHUNK_H_

#include <glm/vec2.hpp>
#include <cstdint>

typedef uint8_t block_id;

class WorldChunk {
public:
    WorldChunk();
    
    static const int SPAN_X = 32;
    static const int SPAN_Y = 256;
    static const int SPAN_Z = 32;
    
    glm::vec2 m_position;
    // Blocks run from (x, y) to (x + 31, y + 31), z = 0 to 255
    block_id m_blocks[SPAN_X][SPAN_Y][SPAN_Z];
    unsigned int m_num_blocks;
};

#endif /* _WORLDCHUNK_H_ */
