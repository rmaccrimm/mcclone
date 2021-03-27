#include "ChunkManager.h"
#include "WorldChunk.h"


ChunkManager::ChunkManager()
{
    for (int i = 0; i < 1; i++) {
	WorldChunk* chunk = new WorldChunk();
	int x = i / 10;
	int z = i % 10;	
        chunk->m_position.x = 32 * x;
	chunk->m_position.z = 32 * z;
	m_chunks.push_back(chunk);
    }
}

ChunkManager::~ChunkManager()
{
    for (int i = 0; i < 1; i++) {
	delete m_chunks[i]; 
    }
}
