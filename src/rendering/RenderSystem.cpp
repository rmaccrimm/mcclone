#include "RenderSystem.h"
#include "EntityAdmin.h"
#include "components/Component.h"

RenderSystem::RenderSystem(EntityAdmin *admin) : m_admin{admin} {}

void RenderSystem::tick()
{
    InputManager* input = m_admin->getInputManager();
    Renderer* renderer = m_admin->getRenderer();
    // Don't know if actually makes sense to have multiple of these entites yet
    for (auto id: m_admin->componentView<
	     CameraComponent,
	     TransformComponent,
	     PlayerControlComponent>())
    {
	auto camera = 
    }
}
