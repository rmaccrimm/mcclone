#ifndef _WORLDCHUNK_H_
#define _WORLDCHUNK_H_

#include <glm/vec2.hpp>
#include <cstdint>

typedef uint8_t block_id;

struct ChunkPosition
{
    int x;
    int z;
};



class WorldChunk {
public:
    WorldChunk(int x, int z);
    
    static const int SPAN_X = 32;
    static const int SPAN_Y = 256;
    static const int SPAN_Z = 32;
    
    ChunkPosition m_position;
    // Blocks run from (x, y) to (x + 31, y + 31), z = 0 to 255
    block_id m_blocks[SPAN_X][SPAN_Y][SPAN_Z];
   
};

#endif /* _WORLDCHUNK_H_ */
