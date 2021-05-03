#include "DebugInfoSystem.h"
#include "EntityAdmin.h"
#include "components/Component.h"

DebugInfoSystem::DebugInfoSystem(EntityAdmin* admin) : m_admin(admin) { }

void DebugInfoSystem::tick()
{
    auto renderer = m_admin->getRenderer();
    // Don't know if it actually makes sense for there can be multiple of these yet
    for (int id :
         m_admin->componentView<CameraComponent>()) {
	
    }
}
