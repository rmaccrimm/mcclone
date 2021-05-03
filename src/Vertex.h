#ifndef _VERTEX_H_
#define _VERTEX_H_

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoords;
};

#endif /* _VERTEX_H_ */
