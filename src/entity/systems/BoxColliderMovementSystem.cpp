#include "BoxColliderMovementSystem.h"
#include "ChunkManager.h"
#include "EntityAdmin.h"
#include "components/Component.h"
#include "util/coordinates.h"
#include <cassert>
#include <cmath>
#include <plog/Log.h>

#include <glm/vec3.hpp>

const float COLLISION_DIST = 0.001f;
BoxColliderComponent CUBE_COLLIDER = { .size = { 0.5, 0.5, 0.5 } };

glm::ivec3 vecFloor(glm::vec3 v) { return glm::ivec3(ifloor(v.x), ifloor(v.y), ifloor(v.z)); }

/*
  Zeroes out the velocity in any direction of any face which is currently in contact with a block
 */
glm::vec3
BoxColliderMovementSystem::calcVelocity(glm::vec3 p_init, glm::vec3 size, glm::vec3 velocity)
{
    if (velocity == glm::vec3(0, 0, 0))
        return velocity;

    auto& chunk_mgr = m_admin.getChunkManager();
    glm::vec3 xn(1, 0, 0);
    glm::vec3 yn(0, 1, 0);
    glm::vec3 zn(0, 0, 1);
    auto pos_faces = p_init + size;
    auto neg_faces = p_init - size;
    auto pos_face_block_coords = vecFloor(pos_faces);
    auto neg_face_block_coords = vecFloor(neg_faces);

    if (glm::dot(velocity, xn) > 0 && (ceil(pos_faces.x) - pos_faces.x) <= COLLISION_DIST) {
        for (int y = neg_faces.y; y <= pos_faces.y; y++) {
            for (int z = neg_faces.z; z <= pos_faces.z; z++) {
                if (chunk_mgr.checkNeighbour(glm::vec3(pos_face_block_coords.x, y, z), xn)) {
                    LOG_DEBUG << "Contact detected in +x direction";
                    velocity.x = 0;
                    break;
                }
            }
        }
    }
    if (glm::dot(velocity, -xn) > 0 && (neg_faces.x - floor(neg_faces.x)) <= COLLISION_DIST) {
        for (int y = neg_faces.y; y <= pos_faces.y; y++) {
            for (int z = neg_faces.z; z <= pos_faces.z; z++) {
                if (chunk_mgr.checkNeighbour(glm::vec3(neg_face_block_coords.x, y, z), -xn)) {
                    LOG_DEBUG << "Contact detected in -x direction";
                    velocity.x = 0;
                    break;
                }
            }
        }
    }
    if (glm::dot(velocity, yn) > 0 && (ceil(pos_faces.y) - pos_faces.y) <= COLLISION_DIST) {
        for (int x = neg_faces.x; x <= pos_faces.x; x++) {
            for (int z = neg_faces.z; z <= pos_faces.z; z++) {
                if (chunk_mgr.checkNeighbour(glm::vec3(x, pos_face_block_coords.y, z), yn)) {
                    LOG_DEBUG << "Contact detected in +y direction";
                    velocity.y = 0;
                    break;
                }
            }
        }
    }
    if (glm::dot(velocity, -yn) > 0 && (neg_faces.y - floor(neg_faces.y)) <= COLLISION_DIST) {
        for (int x = neg_faces.x; x <= pos_faces.x; x++) {
            for (int z = neg_faces.z; z <= pos_faces.z; z++) {
                if (chunk_mgr.checkNeighbour(glm::vec3(x, neg_face_block_coords.y, z), -yn)) {
                    LOG_DEBUG << "Contact detected in -y direction";
                    velocity.y = 0;
                    break;
                }
            }
        }
    }
    if (glm::dot(velocity, zn) > 0 && (ceil(pos_faces.z) - pos_faces.z) <= COLLISION_DIST) {
        for (int x = neg_faces.x; x <= pos_faces.x; x++) {
            for (int y = neg_faces.y; y <= pos_faces.y; y++) {
                if (chunk_mgr.checkNeighbour(glm::vec3(x, y, pos_face_block_coords.z), zn)) {
                    LOG_DEBUG << "Contact detected in +z direction";
                    velocity.z = 0;
                    break;
                }
            }
        }
    }
    if (glm::dot(velocity, -zn) > 0 && (neg_faces.z - floor(neg_faces.z)) <= COLLISION_DIST) {
        for (int x = neg_faces.x; x <= pos_faces.x; x++) {
            for (int y = neg_faces.y; y <= pos_faces.y; y++) {
                if (chunk_mgr.checkNeighbour(glm::vec3(x, y, neg_face_block_coords.z), -zn)) {
                    LOG_DEBUG << "Contact detected in -z direction";
                    velocity.z = 0;
                    break;
                }
            }
        }
    }
    return velocity;
}

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
  Uses a binary search between the initial and final positions of the moving collider to find
  the first point at which the two do not collide (i.e. are closer than COLLISION_DISTANCE)
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

    // Bring dist to just UNDER collision dist to make sure contact detected next frmae
    while (diff >= COLLISION_DIST) {
        mid = (high + low) / 2;
        p_mid = moving_p0 + (mid * (moving_p1 - moving_p0));
        if (detectCollision(p_mid, moving_size, immovable_pos, immovable_size)) {
            high = mid;
            p_high = p_mid;
        } else {
            low = mid;
            p_low = p_mid;
        }
        diff = glm::length(p_high - p_low);
    }
    return p_low;
}

/*
  Resolve any collisions that occur after moving
 */
glm::vec3
BoxColliderMovementSystem::calcFinalPosition(glm::vec3 p_init, glm::vec3 p_final, glm::vec3 size)
{
    auto& chunk_mgr = m_admin.getChunkManager();

    // Block coordinates the collider faces are in
    auto pos_faces = vecFloor(p_final + size);
    auto neg_faces = vecFloor(p_final - size);

    for (int x = neg_faces.x; x <= pos_faces.x; x++) {
        for (int y = neg_faces.y; y <= pos_faces.y; y++) {
            for (int z = neg_faces.z; z <= pos_faces.z; z++) {
                glm::vec3 cube_pos(x + 0.5, y + 0.5, z + 0.5);
                if (chunk_mgr.checkNeighbour(cube_pos, glm::vec3(0.0, 0.0, 0.0))
                    && detectCollision(p_final, size, cube_pos, CUBE_COLLIDER.size)) {
                    p_final = resolveCollision(p_init, p_final, size, cube_pos, CUBE_COLLIDER.size);
                    LOG_DEBUG << "Collision detected. Corrected position to (" << p_final.x << ", "
                              << p_final.y << ", " << p_final.z << ")";
                }
            }
        }
    }
    return p_final;
}

void BoxColliderMovementSystem::tick()
{
    for (int id :
         m_admin.componentView<BoxColliderComponent, TransformComponent, PhysicsComponent>()) {
        auto& chunk_mgr = m_admin.getChunkManager();
        auto collider = m_admin.getComponent<BoxColliderComponent>(id);
        auto& phys = m_admin.getComponent<PhysicsComponent>(id);
        auto& transform = m_admin.getComponent<TransformComponent>(id);

        auto p0 = transform.m_position;
        phys.velocity = calcVelocity(p0, collider.size, phys.velocity);

        // TODO - get thiis from Timer
        float dt = 1.0f / 60.0f;
        auto p1 = p0 + dt * phys.velocity;
        transform.m_position = calcFinalPosition(p0, p1, collider.size);
    }
}
