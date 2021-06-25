#include "CollisionSystem.h"
#include "EntityAdmin.h"
#include "ChunkManager.h"
#include "components/Component.h"
#include "util/coordinates.h"

#include <glm/vec3.hpp>

glm::ivec3 vecFloor(glm::vec3 v) { return glm::ivec3(ifloor(v.x), ifloor(v.y), ifloor(v.z)); }

void CollisionSystem::tick()
{
    for (int id :
         m_admin.componentView<BoxColliderComponent, TransformComponent, PhysicsComponent>()) {
        auto& chunk_mgr = m_admin.getChunkManager();
        auto& collider = m_admin.getComponent<BoxColliderComponent>(id);
        auto phys = m_admin.getComponent<PhysicsComponent>(id);
        auto transform = m_admin.getComponent<TransformComponent>(id);

        // Determine where the collider WOULD be if we move it accord to it's physics
        // TODO - get this from Timer
        float dt = 1.0f / 60.0f;
        auto p = transform.m_position + dt * phys.velocity;

        auto pos_faces = vecFloor(p + collider.size);
        auto neg_faces = vecFloor(p - collider.size);

	collider.collision_detected = false;

        // Very lazy implementation to see which faces are in filled spaces
        for (int x = neg_faces.x; x <= pos_faces.x; x++) {
            for (int y = neg_faces.y; y <= pos_faces.y; y++) {
                for (int z = neg_faces.z; z <= pos_faces.z; z++) {
		    if (chunk_mgr.checkNeighbour(glm::vec3(x, y, z), glm::vec3(0, 0, 0))) {
			collider.collision_detected = true;
			break;
		    }
		}
            }
        }
    }
}
