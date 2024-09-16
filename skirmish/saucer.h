#pragma once

#include "actor.h"

namespace skirmish {

struct Saucer : Actor {

	Saucer();

private:
	int m_changeDirTimeout = 1;
	SGD_Real m_velX = 0;
	SGD_Real m_velY = 0;
	SGD_Real m_velZ = 0;

	void onUpdate() override;
};

} // namespace skirmish
