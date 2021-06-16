#include "utils.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

using v = glm::vec3;
using iv = glm::ivec3;

TEST_CASE("Internal coordinate calc", "[utils]")
{
    REQUIRE(internalCoord(v(0, 128, 0)) == iv(0, 128, 0));
    REQUIRE(internalCoord(v(31.9, 0, 0)) == iv(31, 0, 0));
    REQUIRE(internalCoord(v(0, 0, 31.9)) == iv(0, 0, 31));
    REQUIRE(internalCoord(v(32.0, 0, 31.0)) == iv(0, 0, 31));
    REQUIRE(internalCoord(v(31.9, -1, 32)) == iv(31, -1, 0));

    REQUIRE(internalCoord(v(-0.9, 0, 0.9)) == iv(31, 0, 0));
    REQUIRE(internalCoord(v(63.9, 0, 64.0)) == iv(31, 0, 0));

    REQUIRE(internalCoord(v(-1.0, 0, -31.0)) == iv(31, 0, 1));
    REQUIRE(internalCoord(v(-1.0, 0, -32.0)) == iv(31, 0, 0));
    REQUIRE(internalCoord(v(-32.1, -12.0, -32.0)) == iv(31, -12, 0));

    REQUIRE(internalCoord(v(10.0, 0, -60.1)) == iv(10, 0, 3));
    REQUIRE(internalCoord(v(-55.0, 0, 55)) == iv(9, 0, 23));
}

TEST_CASE("Chunk coordinate calc", "[utils]")
{
    REQUIRE(chunkCoord(v(0, 0, 0)) == iv(0, 0, 0));
    REQUIRE(chunkCoord(v(31.9, 0, 0)) == iv(0, 0, 0));
    REQUIRE(chunkCoord(v(0, 0, 31.9)) == iv(0, 0, 0));
    REQUIRE(chunkCoord(v(31.0, 0, 31.0)) == iv(0, 0, 0));
    REQUIRE(chunkCoord(v(31.9, -1, 31.9)) == iv(0, 0, 0));

    REQUIRE(chunkCoord(v(0, 0, 32.0)) == iv(0, 0, 32));
    REQUIRE(chunkCoord(v(63.9, 0, 63.9)) == iv(32, 0, 32));

    REQUIRE(chunkCoord(v(-1.0, 0, 0.0)) == iv(-32, 0, 0));
    REQUIRE(chunkCoord(v(-1.0, 0, -32.0)) == iv(-32, 0, -32));
    REQUIRE(chunkCoord(v(-32.1, -12.0, -32.0)) == iv(-64, 0, -32));

    REQUIRE(chunkCoord(v(10.0, 0, -32.1)) == iv(0, 0, -64));
    REQUIRE(chunkCoord(v(-55.0, 0, 32)) == iv(-64, 0, 32));
}
