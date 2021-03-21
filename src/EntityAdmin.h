#ifndef ENTITYADMIN_H
#define ENTITYADMIN_H

#include <vector>
#include <unordered_map>
#include <ranges>

#include "components/Component.h"

class InputManager;
class Renderer;

class EntityAdmin
{
public:
    EntityAdmin(InputManager *input_mgr, Renderer *renderer);

    /* Creates a new entity with the given list of component types. All components are 
       default-initialized and can only be modified after creation with getComponent
     */
    template<class ...Types>
    int createEntity()
    {
	Entity entity;
	int id = m_id_counter++;
	entity.m_tag = getTag<Types...>();
	((m_comp_pool[m_comp_counter] = Types(),
	  entity.m_components.push_back(m_comp_counter++)), ...);
	m_entity_map[id] = entity;
	return id;
    }

    /* Given an entity id, produces a pointer to the entity component with the given type, 
       or nullptr if the entity does not contain a component of that type 
     */
    template<class T>
    T* getComponent(int entity_id)
    {
	for (auto i: m_entity_map[entity_id].m_components) {
	    if (std::get_if<T>(&m_comp_pool[i])) {
		return &std::get<T>(m_comp_pool[i]);
	    }
	}
	return nullptr;
    }

    /* Produces a view returning the id of each element that contains only the given
       set of component types. Intended for easily obtaining desired components in systems
       with for (:) syntax
     */ 
    template<class ...Types>
    auto componentView() {
	int tag = getTag<Types...>();
	auto tag_filter = [=](const auto& p){ return p.second.m_tag == tag; };
	return m_entity_map | std::views::filter(tag_filter) | std::views::keys;
    }

    InputManager* getInputManager();
    Renderer* getRenderer();

private:
    class Entity
    {
    public:
	int m_tag;
	std::vector<int> m_components;
    };

    template<class ...Types>
    int getTag() {
	return (Types::TAG | ...);
    }

    int m_id_counter = 0;
    std::unordered_map<int, Entity> m_entity_map;
    
    // Will need a more proper pool allocator once entity deletion is added
    int m_comp_counter;
    Component m_comp_pool[100];

    // TODO - should these be owned/created by EntityManager or passed in?
    // Alternatively, do they need to be here? Could systems just store a pointer?
    InputManager* m_input_mgr;
    Renderer* m_renderer;
};

#endif
