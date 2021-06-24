#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <variant>
#include "CameraComponent.h"
#include "TransformComponent.h"
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

