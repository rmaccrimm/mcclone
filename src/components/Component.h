#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <variant>
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "PlayerControlComponent.h"

typedef std::variant<
    CameraComponent,
    TransformComponent,
    PlayerControlComponent>
Component;

// struct Component
// {
//     template <class T> void assign() {
// 	m_tag = T::TAG;
// 	m_variant = T();
//     }
//     int m_tag = 0;
//     ComponentVariant m_variant;    
// };

#endif /* _COMPONENT_H_ */

