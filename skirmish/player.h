#pragma once

#include "actor.h"

namespace skirmish {

struct Player : Actor {

	Player();

private:
	SGD_Camera m_camera{};
	float m_rvy{};
	float m_vz{};

	void onUpdate() override;

	void onDestroy() override;
};

}
