// Template implementation file
// (include is just for linting)
#include "EntityAdmin.h"
#include <exception>
#include <typeinfo>
#include <string>

template <class... Types> int EntityAdmin::createEntity()
{
    Entity entity;
    int id = m_id_counter++;
    entity.m_tag = getTag<Types...>();
    ((m_comp_pool[m_comp_counter] = Types(), entity.m_components.push_back(m_comp_counter++)), ...);
    m_entity_map[id] = entity;
    return id;
}

template <class T> T& EntityAdmin::getComponent(int entity_id)
{
    for (auto i : m_entity_map[entity_id].m_components) {
        if (std::holds_alternative<T>(m_comp_pool[i])) {
            return std::get<T>(m_comp_pool[i]);
        }
    }
    std::string msg
        = "Entity " + std::to_string(entity_id) + " has no component of type " + typeid(T).name();
    throw std::invalid_argument(msg);
}

template <class... Types> auto EntityAdmin::componentView()
{
    int tag = getTag<Types...>();
    auto tag_filter = [=](const auto& p) { return (p.second.m_tag & tag) == tag; };
    return m_entity_map | std::views::filter(tag_filter) | std::views::keys;
}
