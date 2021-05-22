#ifndef _RENDEROBJECT_H_
#define _RENDEROBJECT_H_

#include <vector>

#include "Vertex.h"
#include "Material.h"

struct RenderObject
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;
};

#endif /* _RENDEROBJECT_H_ */
