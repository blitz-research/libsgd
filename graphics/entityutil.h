#pragma once

#include "entity.h"

namespace sgd {

void transform(Entity* entity, CVec4f trans, CVec3f rot);

void move(Entity* entity, CVec3f trans);

void turn(Entity* entity, CVec3f rrot);

}
