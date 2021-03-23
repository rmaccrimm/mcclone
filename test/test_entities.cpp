#include <catch2/catch.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <set>

#include "EntityAdmin.h"
#include "components/Component.h"


TEST_CASE( "Create/Get entity components", "[EntityAdmin]" )
{
    EntityAdmin admin(nullptr, nullptr, nullptr);
    int id = admin.createEntity<TransformComponent, CameraComponent>();

    auto t = admin.getComponent<TransformComponent>(id);
    auto c = admin.getComponent<CameraComponent>(id);
    auto p = admin.getComponent<PlayerControlComponent>(id);

    // Default to identiy matrices
    REQUIRE( t->m_position == glm::vec3() );
    REQUIRE( c->m_projection == glm::mat4(1.0) );
    // Entity has no player control component
    REQUIRE( p == nullptr );
}

TEST_CASE("Iterate over sets of components", "[EntityAdmin]")
{
    EntityAdmin admin(nullptr, nullptr, nullptr);
    int id0 = admin.createEntity<TransformComponent, CameraComponent, PlayerControlComponent>();
    int id1 = admin.createEntity<TransformComponent, CameraComponent>();
    int id2 = admin.createEntity<TransformComponent, CameraComponent>();

    // Must have all components
    std::set<int> s;
    for (int id: admin.componentView<CameraComponent> ()) {
	s.insert(id);
    }
    REQUIRE( s == std::set<int>{} );
    for (int id: admin.componentView<PlayerControlComponent> ()) {
	s.insert(id);
    }
    REQUIRE( s == std::set<int>{} );
    for (int id: admin.componentView<TransformComponent> ()) {
	s.insert(id);
    }
    REQUIRE( s == std::set<int>{} );
    for (int id: admin.componentView<TransformComponent, CameraComponent, PlayerControlComponent>()) {
	s.insert(id);
    }
    REQUIRE( s == std::set<int>{id0} );

    // Order doesn't matter
    s = std::set<int> {};
    for (int id: admin.componentView<CameraComponent, TransformComponent> ()) {
	s.insert(id);
    }
    REQUIRE( s == std::set<int>{id1, id2} );
}
