#ifndef _CHUNKMANAGER_H_
#define _CHUNKMANAGER_H_

#include <vector>

class WorldChunk;

class ChunkManager
{
public:
    ChunkManager();
    ~ChunkManager();
    std::vector<WorldChunk*> m_chunks;
};


#endif /* _CHUNKMANAGER_H_ */
