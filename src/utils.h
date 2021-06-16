#ifndef _UTILS_H_
#define _UTILS_H_

#include "WorldChunk.h"
#include <glm/vec3.hpp>

#include <sstream>
#include <cmath>

std::ostream& operator<<(std::ostream& os, glm::vec3 const& value)
{
    std::stringstream ss;
    ss << "(" << value.x << ", " << value.y << ", " << value.z << ")";
    os << ss.str();
    return os;
}

inline int roundToLowerMultiple(float x, int multiple)
{
    int floor = int(x);
    if (floor > x) floor--;
    return (floor - (floor < 0 ? multiple - 1 : 0)) / multiple * multiple;
}

int getRemainder(float x, int multiple) { return x - roundToLowerMultiple(x, multiple); }

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
