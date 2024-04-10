#pragma once

#include "entity.h"

namespace sgd {

inline void setPosition(Entity* entity, CVec3r pos) {
	entity->setWorldPosition(pos);
}

inline Vec3r position(const Entity* entity) {
	return entity->worldPosition();
}

inline void setRotation(Entity* entity, CVec3r rot) {
	auto basis = Mat3r::rotation(rot * real(degreesToRadians));
	entity->setWorldBasis(basis);
}

inline Vec3r rotation( const Entity* entity) {
	return rotation(entity->worldBasis());
}

inline void setScale(Entity* entity, CVec3r scl) {
	entity->setWorldScale(scl);
}

inline void move(Entity* entity, CVec3r trans) {
	entity->setWorldPosition(entity->worldPosition() + entity->worldMatrix().r * trans);
}

inline void turn(Entity* entity, CVec3r rot) {
	auto basis = Mat3r::rotation(rot * real(degreesToRadians));
	entity->setWorldBasis(entity->worldBasis() * basis);
}

inline void translate(Entity* entity, CVec3r trans) {
	entity->setWorldPosition(trans + entity->worldPosition());
}

inline void rotate(Entity* entity, CVec3r rot) {
	auto basis = Mat3r::rotation(rot * real(degreesToRadians));
	entity->setWorldBasis(basis * entity->worldBasis());
}

inline void scale(Entity* entity, CVec3r scl) {
	entity->setWorldScale(scl * entity->worldScale());
}

} // namespace sgd
