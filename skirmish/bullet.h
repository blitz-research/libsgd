#pragma once

#include "actor.h"

namespace skirmish {

struct Player;

struct Bullet : Actor {

	explicit Bullet(Player* player);

private:
	SGD_Collider m_collider;
	int m_timeout{180};

	void onUpdate() override;
};

}
