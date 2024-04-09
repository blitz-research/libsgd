#pragma once

#include "entity.h"

namespace sgd {

inline void setPosition(Entity* entity, CVec3f pos) {
	entity->setWorldPosition(pos);
}

inline Vec3f position(const Entity* entity) {
	return entity->worldPosition();
}

inline void setRotation(Entity* entity, CVec3f rot) {
	auto basis = Mat3f::rotation(rot * degreesToRadians);
	entity->setWorldBasis(basis);
}

inline Vec3f rotation( const Entity* entity) {
	return rotation(entity->worldBasis());
}

inline void setScale(Entity* entity, CVec3f scl) {
	entity->setWorldScale(scl);
}

inline void move(Entity* entity, CVec3f trans) {
	entity->setWorldPosition(entity->worldPosition() + entity->worldMatrix().r * trans);
}

inline void turn(Entity* entity, CVec3f rot) {
	auto basis = Mat3f::rotation(rot * degreesToRadians);
	entity->setWorldBasis(entity->worldBasis() * basis);
}

inline void translate(Entity* entity, CVec3f trans) {
	entity->setWorldPosition(trans + entity->worldPosition());
}

inline void rotate(Entity* entity, CVec3f rot) {
	auto basis = Mat3f::rotation(rot * degreesToRadians);
	entity->setWorldBasis(basis * entity->worldBasis());
}

inline void scale(Entity* entity, CVec3f scl) {
	entity->setWorldScale(scl * entity->worldScale());
}

} // namespace sgd
