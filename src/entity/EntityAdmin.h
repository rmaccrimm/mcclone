#ifndef _ENTITYADMIN_H_
#define _ENTITYADMIN_H_

#include <optional>
#include <ranges>
#include <unordered_map>
#include <vector>

#include "components/Component.h"

class InputManager;
class Renderer;
class ChunkManager;

class EntityAdmin {
public:
    EntityAdmin(InputManager* input_mgr, Renderer* renderer, ChunkManager* chunk_mgr);
    EntityAdmin(const EntityAdmin&) = delete;
    EntityAdmin& operator=(const EntityAdmin&) = delete;

    /* Creates a new entity with the given list of component types. All components are
       default-constructed and can only be modified after creation with getComponent
     */
    template <class... Types> int createEntity();

    /* Given an entity id, returns a reference to the component with the given type, or throws
       std::invalid_argument if the entity does not contain a component of that type
     */
    template <class T> T& getComponent(int entity_id);

    /* Produces a std::ranges::view returning the id of each element that contains only the
       given set of component types. Intended for easily obtaining desired components in
       systems with for (:) syntax
     */
    template <class... Types> auto componentView();

    InputManager& getInputManager();
    Renderer& getRenderer();
    ChunkManager& getChunkManager();

private:
    class Entity {
    public:
        int m_tag;
        std::vector<int> m_components;
    };

    template <class... Types> int getTag() { return (Types::TAG | ...); }

    int m_id_counter = 0;
    std::unordered_map<int, Entity> m_entity_map;

    // Will need a more proper pool allocator once entity deletion is added
    int m_comp_counter;
    Component m_comp_pool[100];

    InputManager* m_input_mgr;
    Renderer* m_renderer;
    ChunkManager* m_chunk_mgr;
};

#include "EntityAdmin.tpp"

#endif /* _ENTITYADMIN_H_ */
