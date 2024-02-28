#include "entityutil.h"

namespace sgd {

void transform(Entity* entity, CVec3f trans, CVec3f rot) {
	entity->setWorldMatrix(entity->worldMatrix() * AffineMat4f::TRS(trans,rot));
}

void move(Entity* entity, CVec3f trans) {
	entity->setWorldMatrix(entity->worldMatrix() * AffineMat4f::translation(trans));
}

void turn(Entity* entity, CVec3f rot) {
	entity->setWorldMatrix(entity->worldMatrix() * AffineMat4f::rotation(rot));
}

}
