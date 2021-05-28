#include <catch2/catch.hpp>
#include <glm/vec3.hpp>

#include "rendering/Renderer.h"

int roundToNegativeInf(int x, int multiple)
{
    return (x - (x < 0 ? multiple - 1 : 0)) / multiple * multiple;
}

int getRemainder(int x, int multiple) { return x - roundToNegativeInf(x, multiple); }

glm::ivec3 chunkCoord(glm::vec3 world_coords)
{
    return glm::ivec3(
        roundToNegativeInf(world_coords.x, 32), 0, roundToNegativeInf(world_coords.z, 32));
}

glm::ivec3 internalCoord(glm::vec3 world_coords)
{
    return glm::ivec3(
        getRemainder(world_coords.x, 32), world_coords.y, getRemainder(world_coords.z, 32));
}

TEST_CASE("Copy vertices/indices to renderer buffer", "[Renderer]")
{
    glm::vec3 pos(16, 59, -4);
    auto chunk = chunkCoord(pos);
    glm::ivec3 expect(0, 0, -32);
    REQUIRE(chunk.x == expect.x);
    REQUIRE(chunk.x == expect.y);
    REQUIRE(chunk.z == expect.z);
    int y = 32 + (-40 % 32);
    REQUIRE(y == 24);
}
