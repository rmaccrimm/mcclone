#include "BoxColliderMovementSystem.h"
#include "ChunkManager.h"
#include "EntityAdmin.h"
#include "components/Component.h"
#include "util/coordinates.h"
#include <cassert>
#include <plog/Log.h>

#include <glm/vec3.hpp>

const float COLLISION_DIST = 0.001f;

glm::ivec3 vecFloor(glm::vec3 v) { return glm::ivec3(ifloor(v.x), ifloor(v.y), ifloor(v.z)); }

inline bool between(float x, float low, float high) { return x >= low && x <= high; }

bool detectCollision(glm::vec3 a_pos, glm::vec3 a_size, glm::vec3 b_pos, glm::vec3 b_size)
{
    auto a_posv_faces = a_pos + a_size;
    auto b_posv_faces = b_pos + b_size;
    auto a_negv_faces = a_pos - a_size;
    auto b_negv_faces = b_pos - b_size;
    return a_posv_faces.x >= b_negv_faces.x && a_negv_faces.x <= b_posv_faces.x
        && a_posv_faces.y >= b_negv_faces.y && a_negv_faces.y <= b_posv_faces.y
        && a_posv_faces.z >= b_negv_faces.z && a_negv_faces.z <= b_posv_faces.z;
}

/*
  Uses a binary search between the initial and final positions of the moving collider to find the
  first point at which the two do not collide (i.e. are closer than COLLISION_DISTANCE)
 */
glm::vec3 resolveCollision(
    glm::vec3 moving_p0,
    glm::vec3 moving_p1,
    glm::vec3 moving_size,
    glm::vec3 immovable_pos,
    glm::vec3 immovable_size)
{
    assert(!detectCollision(moving_p0, moving_size, immovable_pos, immovable_size));
    assert(detectCollision(moving_p1, moving_size, immovable_pos, immovable_size));
    
    float low = 0.0;
    float mid = 0.0;
    float high = 1.0;
    glm::vec3 p_low = moving_p0;
    glm::vec3 p_mid = moving_p0;
    glm::vec3 p_high = moving_p1;
    float diff = glm::length(moving_p1 - moving_p0);
    
    while (diff > COLLISION_DIST) {
        mid = (high + low) / 2;
        p_mid = moving_p0 + (mid * (moving_p1 - moving_p0));
        if (detectCollision(p_mid, moving_size, immovable_pos, immovable_size)) {
	    high = mid;
	    p_high = p_mid;
	}
	else {
	    low = mid;
	    p_low = p_mid;
	}
	diff = glm::length(p_high - p_low);
    }
    return p_low;
}

void BoxColliderMovementSystem::tick()
{
    for (int id :
         m_admin.componentView<BoxColliderComponent, TransformComponent, PhysicsComponent>()) {
        auto& chunk_mgr = m_admin.getChunkManager();
        auto collider = m_admin.getComponent<BoxColliderComponent>(id);
        auto phys = m_admin.getComponent<PhysicsComponent>(id);
        auto& transform = m_admin.getComponent<TransformComponent>(id);

        // TODO - get this from Timer
        float dt = 1.0f / 60.0f;
        auto p0 = transform.m_position;
        auto& p1 = transform.m_position;
        p1 = p0 + dt * phys.velocity;

        BoxColliderComponent cube_collider = { .size = { 0.5, 0.5, 0.5 } };

        // Check all blocks touched by one of the collider's faces
        auto pos_faces = vecFloor(p1 + collider.size);
        auto neg_faces = vecFloor(p1 - collider.size);
        for (int x = neg_faces.x; x <= pos_faces.x; x++) {
            for (int y = neg_faces.y; y <= pos_faces.y; y++) {
                for (int z = neg_faces.z; z <= pos_faces.z; z++) {
                    glm::vec3 cube_pos(x + 0.5, y + 0.5, z + 0.5);
                    if (chunk_mgr.checkNeighbour(cube_pos, glm::vec3(0.0, 0.0, 0.0))
                        && detectCollision(p1, collider.size, cube_pos, cube_collider.size)) {
                        LOG_DEBUG << "Collision detected";
                        p1 = resolveCollision(p0, p1, collider.size, cube_pos, cube_collider.size);
                    }
                }
            }
        }
    }
}
