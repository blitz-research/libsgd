#include "game.h"

#include "ground.h"
#include "player.h"
#include "tower.h"

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
	int size = 50;
	int step = 10;
	for (int z = -size; z <= size; z += step) {
		for (int x = -size; x <= size; x += step) {
			new Tower((float)x + size/2, (float)z + size/2);
		}
	}
	new Player();
}

void Game::update() {

	sgd_UpdateColliders();

	sgd_Update3DAudio();

	m_world->update();
}

} // namespace skirmish
