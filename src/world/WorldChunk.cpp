#include "WorldChunk.h"

WorldChunk::WorldChunk(glm::vec3 _origin, unsigned int _render_obj_id) :
    origin { _origin }, render_obj_id { _render_obj_id }, blocks { 0 }
{
}
