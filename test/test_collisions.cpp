#include <catch2/catch.hpp>

#include "systems/BoxColliderMovementSystem.h"

using v = glm::vec3;
using iv = glm::ivec3;

TEST_CASE("Test collision detection", "[Collisions]")
{
    // One inside of the other
    REQUIRE(detectCollision(v(0, 0, 0), v(.5, .5, .5), v(0, 0, 0), v(1, 1, 1)));
    REQUIRE(detectCollision(v(0, 0, 0), v(1, 1, 1), v(0, 0, 0), v(0.5, .5, .5)));
}

TEST_CASE("Test collision resolution", "[Collisions]")
{
    // REQUIRE_THROWS_AS(admin.getComponent<PlayerControlComponent>(id), std::invalid_argument);
}
