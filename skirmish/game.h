#pragma once

#include "std.h"
#include "world.h"

namespace skirmish {

struct Game {

	Game();
	~Game();

	World* world() const {
		return m_world;
	}

	void begin();

	void update();

private:
	World* m_world;
};

inline Game* game;

} // namespace skirmish
