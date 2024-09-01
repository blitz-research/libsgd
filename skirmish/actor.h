#pragma once

#include "std.h"

namespace skirmish {

struct World;

enum struct ActorType {
	destroyed = -1,
	ground,
	tower,
	bullet,
	player,
	saucer,
};
static constexpr int maxActorTypes = 16;

struct Actor {

	ActorType type() const {
		return m_type;
	}

	bool active() const {
		return m_type != ActorType::destroyed;
	}

	SGD_Entity entity()const{
		return  m_entity;
	}

	void destroy();

	void update() {
		onUpdate();
	}

protected:
	SGD_Entity m_entity{};

	explicit Actor(ActorType type);
	virtual ~Actor() = default;

	virtual void onUpdate();
	virtual void onDestroy();

private:
	friend class World;

	ActorType m_type;
};

int onInitActorTypes(const std::function<void()>& initFunc);

void initActorTypes();

} // namespace skirmish
