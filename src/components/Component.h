#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <variant>
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "PlayerControlComponent.h"
#include "CameraControlComponent.h"
#include "BoxColliderComponent.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "MovementComponent.h"

typedef std::variant<
    CameraComponent,
    TransformComponent,
    PlayerControlComponent,
    CameraControlComponent,
    BoxColliderComponent,
    InputComponent,
    PhysicsComponent,
    MovementComponent
> Component;

#endif /* _COMPONENT_H_ */

