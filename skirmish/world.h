#pragma once

#include "actor.h"

namespace skirmish {

struct World {

	const std::vector<Actor*>& actorsOfType(ActorType type) const {
		return m_actors[(int)type];
	}

	void add(Actor* actor);

	void update();

private:
	std::vector<Actor*> m_actors[maxActorTypes];
};

} // namespace skirmish
