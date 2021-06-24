#include "ChunkLoadingSystem.h"
#include "ChunkManager.h"
#include "EntityAdmin.h"

#include <plog/Log.h>

ChunkLoadingSystem::ChunkLoadingSystem(EntityAdmin* admin) : m_admin { admin } { }

bool withinChunk(glm::vec3 coord, glm::vec3 chunk_origin)
{
    return chunk_origin.x <= coord.x && coord.x < chunk_origin.x + WorldChunk::SPAN_X
        && chunk_origin.z <= coord.z && coord.z < chunk_origin.z + WorldChunk::SPAN_Z;
}

void ChunkLoadingSystem::tick()
{
    auto chunk_mgr = m_admin->getChunkManager();
    for (int id :
         m_admin->componentView<TransformComponent, PlayerControlComponent>()) {
        auto player_position = m_admin->getComponent<TransformComponent>(id)->m_position;
	
        if (!withinChunk(player_position, chunk_mgr->getCenterChunk()->origin)) {
            chunk_mgr->reloadChunks(player_position);
        }
    }
}
