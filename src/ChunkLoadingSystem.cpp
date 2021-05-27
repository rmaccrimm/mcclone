#include "ChunkLoadingSystem.h"
#include "EntityAdmin.h"
#include "ChunkManager.h"

ChunkLoadingSystem::ChunkLoadingSystem(EntityAdmin* admin) : m_admin { admin } { }

void ChunkLoadingSystem::tick()
{
    auto chunk_mgr = m_admin->getChunkManager();
    for (int id : m_admin->componentView<TransformComponent, PlayerControlComponent>()) {
        auto& chunk = chunk_mgr->getCenterChunk();
	
    }
}
