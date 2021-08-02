#ifndef _UTILS_H_
#define _UTILS_H_

#include "WorldChunk.h"
#include <glm/vec3.hpp>

#include <cmath>
#include <sstream>

inline std::ostream& operator<<(std::ostream& os, glm::vec3 const& value)
{
    std::stringstream ss;
    ss << "(" << value.x << ", " << value.y << ", " << value.z << ")";
    os << ss.str();
    return os;
}

/*
  Rounds towards negative infinity
 */
inline int ifloor(float x)
{
    int floor = int(x);
    if (floor > x)
        floor--;
    return floor;
}

/*
  Rounds towards positive infinity
 */
inline int iceil(float x)
{
    int ceil = int(x);
    if (ceil < x)
	ceil++;
    return ceil;
}

/*
  Rounds x to the nearest lower multiple of n (rounds towards negative infinity)
 */
inline int roundToLowerMultiple(float x, int n)
{
    int floor = ifloor(x);
    return (floor - (floor < 0 ? n - 1 : 0)) / n * n;
}

/*
  Get the integer remainder after rounding x to the nearest lower multiple of n (rounds towards
  negative infinity)
 */
inline int getRemainder(float x, int n) { return x - roundToLowerMultiple(x, n); }

inline glm::ivec3 chunkCoord(glm::vec3 world_coord)
{
    return glm::ivec3(
        roundToLowerMultiple(world_coord.x, WorldChunk::SPAN_X),
        0,
        roundToLowerMultiple(world_coord.z, WorldChunk::SPAN_Z));
}

inline glm::ivec3 internalCoord(glm::vec3 world_coord)
{
    return glm::ivec3(
        getRemainder(world_coord.x, WorldChunk::SPAN_X),
        world_coord.y,
        getRemainder(world_coord.z, WorldChunk::SPAN_Z));
}

#endif /* _UTILS_H_ */
