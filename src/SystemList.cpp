#include "SystemList.h"
#include "CameraControlSystem.h"
#include "CameraMovementSystem.h"
#include "ChunkLoadingSystem.h"
#include "ChunkRenderSystem.h"
#include "EntityAdmin.h"
#include "MovementSystem.h"
#include "PhysicsSystem.h"
#include "PlayerControlSystem.h"

SystemList::SystemList(EntityAdmin& admin)
{
    m_systems.push_back(std::make_unique<PlayerControlSystem>(admin));
    m_systems.push_back(std::make_unique<CameraControlSystem>(admin));
    m_systems.push_back(std::make_unique<CameraMovementSystem>(admin));
    m_systems.push_back(std::make_unique<PhysicsSystem>(admin));
    m_systems.push_back(std::make_unique<MovementSystem>(admin));
    m_systems.push_back(std::make_unique<ChunkLoadingSystem>(admin));
    m_systems.push_back(std::make_unique<ChunkRenderSystem>(admin));
}

void SystemList::runAll()
{
    for (std::unique_ptr<System>& s : m_systems) {
        s->tick();
    }
}
