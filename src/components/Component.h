#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <variant>
#include "CameraComponent.h"
#include "TransformComponent.h"

typedef std::variant<
    CameraComponent,
    TransformComponent>
ComponentVariant;

struct Component
{
    Component() : m_tag(0) {}
    template <class T> void assign() {
	m_tag = T::TAG;
	m_variant = T();
    }
    int m_tag;
    ComponentVariant m_variant;    
};

#endif /* _COMPONENT_H_ */

