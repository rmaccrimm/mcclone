#include <catch2/catch.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <set>
#include <exception>

#include "EntityAdmin.h"
#include "components/Component.h"

TEST_CASE("Create/Get entity components", "[EntityAdmin]")
{
    EntityAdmin admin(nullptr, nullptr, nullptr);
    int id = admin.createEntity<TransformComponent, CameraComponent>();

    auto t = admin.getComponent<TransformComponent>(id);
    auto c = admin.getComponent<CameraComponent>(id);
    // Default to identiy matrices
    REQUIRE(t.m_position == glm::vec3());
    REQUIRE(c.m_projection == glm::mat4(1.0));

    REQUIRE_THROWS_AS(admin.getComponent<PlayerControlComponent>(id), std::invalid_argument);
}

TEST_CASE("Iterate over sets of components", "[EntityAdmin]")
{
    EntityAdmin admin(nullptr, nullptr, nullptr);
    int id0 = admin.createEntity<TransformComponent, CameraComponent, PlayerControlComponent>();
    int id1 = admin.createEntity<TransformComponent, CameraComponent>();
    int id2 = admin.createEntity<TransformComponent, CameraComponent>();

    // Any entity containing given components is matched
    std::set<int> s;
    for (int id : admin.componentView<CameraComponent>()) {
        s.insert(id);
    }
    REQUIRE(s == std::set<int> { id0, id1, id2 });

    s.clear();
    for (int id : admin.componentView<PlayerControlComponent>()) {
        s.insert(id);
    }
    REQUIRE(s == std::set<int> { id0 });

    s.clear();
    for (int id : admin.componentView<TransformComponent>()) {
        s.insert(id);
    }
    REQUIRE(s == std::set<int> { id0, id1, id2 });

    s.clear();
    for (int id :
         admin.componentView<TransformComponent, CameraComponent, PlayerControlComponent>()) {
        s.insert(id);
    }
    REQUIRE(s == std::set<int> { id0 });

    // Order doesn't matter
    s.clear();
    for (int id : admin.componentView<CameraComponent, TransformComponent>()) {
        s.insert(id);
    }
    REQUIRE(s == std::set<int> { id0, id1, id2 });
}

TEST_CASE("Get component by copy", "[EntityAdmin]")
{
    EntityAdmin admin(nullptr, nullptr, nullptr);
    int id0 = admin.createEntity<TransformComponent>();

    auto &t = admin.getComponent<TransformComponent>(id0);
    t.m_position = glm::vec3(-71, 0, 33);

    auto s = admin.getComponent<TransformComponent>(id0);
    s.m_position = glm::vec3(-11, 12, 19);

    auto q = admin.getComponent<TransformComponent>(id0);
    REQUIRE(q.m_position.x == -71);
    REQUIRE(q.m_position.y == 0);
    REQUIRE(q.m_position.z == 33);
}

