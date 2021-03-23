#include "ChunkManager.h"
#include "WorldChunk.h"


ChunkManager::ChunkManager()
{
    for (int i = 0; i < 0; i++) {
	// WorldChunk* chunk = new WorldChunk();
	// m_chunks.push_back(chunk);
	// int x = i / 10;
	// int z = i % 10;
	// m_chunks[i]->m_position.x = 32 * x;
	// m_chunks[i]->m_position.z = 32 * z;
    }
}

ChunkManager::~ChunkManager()
{
    for (int i = 0; i < 0; i++) {
	delete m_chunks[i]; 
    }
}
