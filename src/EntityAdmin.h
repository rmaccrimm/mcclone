#ifndef ENTITYADMIN_H
#define ENTITYADMIN_H

#include <vector>
#include <unordered_map>

#include "components/Component.h"

/*  Requirements:
    - Need to be able to create an entity, and assign it various components
    - Quickly determine if an entity matches a given signature (list of components)
      -> can use a bit mask with bits for each component type, (but then limited to 64 
         component types)
    - Assign components out of a static pool
      -> BaseComponent pointers that are type-casted? Some kind of union type?
 */
class EntityAdmin
{
public:
    EntityAdmin();

    template<class ...Types>
    int createEntity()
    {
	int id = __COUNTER__;
	Entity entity;
	((m_comp_pool[m_comp_counter].assign<Types>(),
	  entity.m_components.push_back(m_comp_counter++)), ...);
	m_entity_map[id] = entity;
	return id;
    }

    template<class T>
    T* getComponent(int entity_id)
    {
	for (auto i: m_entity_map[entity_id].m_components) {
	    if (std::get_if<T>(&m_comp_pool[i].m_variant)) {
		return &std::get<T>(m_comp_pool[i].m_variant);
	    }
	}
	return nullptr;
    }
    
// private:
    class Entity
    {
    public:
	int m_tag;
	std::vector<int> m_components;
    };

    std::unordered_map<int, Entity> m_entity_map;
    Component m_comp_pool[100];
    // InputManager m_input;
    int m_comp_counter;
};

#endif
