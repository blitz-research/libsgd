#include "actor.h"
#include "game.h"

namespace skirmish {

namespace {

std::vector<std::function<void()>>* g_initFuncs;

}

Actor::Actor(ActorType type) : m_type(type) {
	game->world()->add(this);
}

void Actor::destroy() {
	assert(m_type != ActorType::destroyed);
	onDestroy();
	m_type = ActorType::destroyed;
}

void Actor::onUpdate() {
}

void Actor::onDestroy() {
	if (!m_entity) return;
	sgd_DestroyEntity(m_entity);
	m_entity = SGD_NULL;
}

int onInitActorTypes(const std::function<void()>& initFunc) {
	if (!g_initFuncs) g_initFuncs = new std::vector<std::function<void()>>();
	g_initFuncs->push_back(initFunc);
	return (int)g_initFuncs->size();
}

void initActorTypes() {
	if (!g_initFuncs) return;
	for (auto& f : *g_initFuncs) f();
}

} // namespace skirmish
