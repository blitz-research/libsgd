#pragma once

#include "actor.h"

namespace skirmish {

struct Tower : Actor {

	Tower(float x,float z);

private:
	SGD_Collider m_collider{};
};

} // namespace skirmish
