#include "audiolistener.h"

#include "../scene/scene.h"
#include "audiospace.h"

#include <audio/exports.h>

namespace sgd {

AudioListener::AudioListener(Entity* entity) : m_srcpos(entity->worldPosition()) {
	attach(entity);
}

void AudioListener::update() {

	auto const& pos = entity()->worldPosition();
	auto vel = pos - m_srcpos;
	m_srcpos = pos;

	update3DAudioListener(pos, entity()->worldBasis().k, entity()->worldBasis().j, vel);
}

void AudioListener::onCreate() {
	entity()->scene()->audioSpace()->add(this);
}

void AudioListener::onDestroy() {
	entity()->scene()->audioSpace()->remove(this);
}

void AudioListener::onReset() {
	m_srcpos = entity()->worldPosition();
}

}
