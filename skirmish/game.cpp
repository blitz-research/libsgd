#include "game.h"

#include "ground.h"
#include "player.h"
#include "tower.h"
#include "saucer.h"

namespace skirmish {

Game::Game() : m_world(new World()) {
}

Game::~Game() {
	delete m_world;
}

void Game::begin() {

	auto light = sgd_CreateDirectionalLight();
	sgd_RotateEntity(light, -45, -45, 0);

	new Ground();

	for (int z = 0; z <= (int)gridSize; z += cellSize) {
		for (int x = -gridSize; x <= (int)gridSize; x += cellSize) {
			new Tower((float)x, (float)z);
		}
	}

	new Player();

	m_newSaucerTimeout = 90;
}

void Game::update() {

	if (!--m_newSaucerTimeout) {

		new Saucer();

		m_newSaucerTimeout = 600;
	}

	sgd_UpdateColliders();

	sgd_Update3DAudio();

	m_world->update();
}

} // namespace skirmish
