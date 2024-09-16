#pragma once

#include "std.h"
#include "world.h"

namespace skirmish {

struct Game {

	static constexpr SGD_Real gridSize = 50.0f;
	static constexpr SGD_Real cellSize = 10.0f;
	static constexpr int numCells = gridSize/cellSize;

	Game();
	~Game();

	World* world() const {
		return m_world;
	}

	void begin();

	void update();

private:
	World* m_world;

	int m_newSaucerTimeout;
};

inline Game* game;

} // namespace skirmish
