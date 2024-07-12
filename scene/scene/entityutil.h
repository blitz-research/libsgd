#pragma once

#include "entity.h"

namespace sgd {

inline void setPosition(Entity* entity, CVec3r pos) {
	entity->setLocalPosition(pos);
}

inline Vec3r position(const Entity* entity) {
	return entity->localPosition();
}

inline void setRotation(Entity* entity, CVec3r rot) {
	entity->setLocalBasis(Mat3r::rotation(rot * real(degreesToRadians)));
}

inline Vec3r rotation(const Entity* entity) {
	return rotation(entity->localBasis());
}

inline void setScale(Entity* entity, CVec3r scl) {
	entity->setLocalScale(scl);
}

inline void move(Entity* entity, CVec3r trans) {
	entity->setLocalPosition(entity->localPosition() + entity->localMatrix().r * trans);
}

inline void turn(Entity* entity, CVec3r rot) {
	entity->setLocalBasis(entity->localBasis() * Mat3r::rotation(rot * real(degreesToRadians)));
}

inline void translate(Entity* entity, CVec3r trans) {
	entity->setLocalPosition(trans + entity->localPosition());
}

inline void rotate(Entity* entity, CVec3r rot) {
	entity->setLocalBasis(Mat3r::rotation(rot * real(degreesToRadians)) * entity->localBasis());
}

inline void scale(Entity* entity, CVec3r scl) {
	entity->setLocalScale(scl * entity->localScale());
}

inline void aim(Entity* entity, CVec3r dst, float roll) {
	auto v = dst - entity->worldPosition();
#if 0
	auto k=normalize(v);
	auto i =cross(k, Vec3r(0,1,0));
	auto j = normalize(cross(i, k));
	i=cross(j, k);
	entity->setWorldBasis({i,j,k});
#endif
	entity->setWorldBasis(Mat3r::rotation({pitch(v),yaw(v),roll * degreesToRadians}));
}

} // namespace sgd
