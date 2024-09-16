#pragma once

#include "actor.h"

namespace skirmish {

struct Player : Actor {

	Player();

private:
	SGD_Camera m_camera{};
	SGD_Real m_rvy{};
	SGD_Real m_vz{};

	void onUpdate() override;

	void onDestroy() override;
};

}
