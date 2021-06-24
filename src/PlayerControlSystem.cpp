#include "PlayerControlSystem.h"
#include "EntityAdmin.h"
#include "InputManager.h"
#include "components/Component.h"

PlayerControlSystem::PlayerControlSystem(EntityAdmin* admin) : m_admin { admin } { }

void PlayerControlSystem::tick()
{
    auto input = m_admin->getInputManager();
    for (int id :
         m_admin->componentView<MovementComponent, TransformComponent, PlayerControlComponent>()) {
        auto move = m_admin->getComponent<MovementComponent>(id);
	// Todo - rotate this to align with player rotation, normalize (?)
        move->direction.x = (int)input->isPressed(Key::RIGHT) - (int)input->isPressed(Key::LEFT);
        move->direction.y = (int)input->isPressed(Key::UP) - (int)input->isPressed(Key::DOWN);
        move->direction.z = (int)input->isPressed(Key::BACK) - (int)input->isPressed(Key::FORWARD);
    }
}
