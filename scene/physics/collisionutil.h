#pragma once

#include "collisionspace.h"

namespace sgd {

SGD_SHARED(Camera);

Vec3r collideRay(const CollisionSpace* space, Vec3r src, Vec3r dst, float radius, uint32_t colliderMask, uint32_t colliderType,
				 Vector<Collision>& collisions);

Vec3r collideRay(const CollisionSpace* space, Vec3r src, Vec3r dst, CVec3f radii, uint32_t colliderMask, uint32_t colliderType,
				 Vector<Collision>& collisions);

Collider* intersectRay(Camera* camera, CVec2f windowsCoords, int colliderMask, Contact& contact);

} // namespace sgd
