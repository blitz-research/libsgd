#pragma once

#include "entity.h"

namespace sgd {

inline void setPosition(Entity* entity, CVec3f pos) {
	entity->setWorldMatrix({entity->worldMatrix().r, pos});
}

inline void setRotation(Entity* entity, CVec3f rot) {
	entity->setWorldMatrix({Mat3f::rotation(rot), entity->worldMatrix().t});
}

inline void move(Entity* entity, CVec3f trans) {
	entity->setWorldMatrix(entity->worldMatrix() * AffineMat4f::translation(trans));
}

inline void turn(Entity* entity, CVec3f rot) {
	entity->setWorldMatrix(entity->worldMatrix() * AffineMat4f::rotation(rot));
}

inline void translate(Entity* entity, CVec3f trans) {
	auto m = AffineMat4f::translation(trans) * entity->worldMatrix();
	entity->setWorldMatrix({entity->worldMatrix().r, m.t});
}

inline void rotate(Entity* entity, CVec3f rot) {
	auto m = AffineMat4f::rotation(rot) * entity->worldMatrix();
	entity->setWorldMatrix({m.r, entity->worldMatrix().t});
}

} // namespace sgd
