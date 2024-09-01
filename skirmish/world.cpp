#include "world.h"

namespace skirmish {

void World::add(Actor* actor) {
	m_actors[(int)actor->type()].push_back(actor);
}

void World::update() {
	for (auto& actors : m_actors) {
		int put = 0;
		for (int get = 0; get < actors.size(); ++get) {
			auto actor = actors[get];
			if(!actor->active()) {
				delete actor;
				continue;
			}
			actors[put++] = actor;
			actor->update();
		}
		actors.resize(put);
	}
}

}
