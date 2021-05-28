#include "WorldChunk.h"

WorldChunk::WorldChunk(glm::ivec3 _origin, unsigned int _render_obj_id) :
    updated {true}, render_obj_id { _render_obj_id }, blocks { 0 }
{
    origin = glm::ivec3(_origin.x, 0, _origin.z);
}
