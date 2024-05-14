#pragma once

#include "collisionspace.h"

namespace sgd {

SGD_SHARED(Camera);

Vec3r collideRay(const CollisionSpace* space, Vec3r src, Vec3r dst, real rradius, uint32_t colliderMask, CollisionResponse response, Vector<Collision>& collisions);

Collider* intersectRay(Camera* camera, CVec2f windowsCoords,int colliderMask, Contact& contact);

}
