#include "audioemitter.h"

#include "../scene/scene.h"
#include "audiospace.h"

#include <audio/exports.h>

namespace sgd {

AudioEmitter::AudioEmitter(Entity* entity) : m_srcpos(entity->worldPosition()) {
	attach(entity);
}

AudioEmitter* AudioEmitter::getOrCreate(Entity* entity) {
	for (EntityListener* listener : entity->listeners()) {
		if (listener->is<AudioEmitter>()) return listener->as<AudioEmitter>();
	}
	return new AudioEmitter(entity);
}

uint32_t AudioEmitter::play3DSound(Sound* sound) {
	auto audio = sgd::play3DSound(sound, entity()->worldPosition(), entity()->worldPosition() - m_srcpos);
	for (auto& i : m_audios) {
		SGD_ASSERT(i != audio); // Audio handles should be unique?
		if (!audioValid(i)) return i = audio;
	}
	m_audios.push_back(audio);
	return audio;
}

void AudioEmitter::onCreate() {
	entity()->scene()->audioSpace()->add(this);
}

void AudioEmitter::onDestroy() {
	entity()->scene()->audioSpace()->remove(this);
}

void AudioEmitter::onReset() {
	m_srcpos = entity()->worldPosition();
}

void AudioEmitter::update() {
	const auto& pos = entity()->worldPosition();
	auto vel = pos - m_srcpos;
	m_srcpos = pos;
	int put = 0;
	for (int get = 0; get < m_audios.size(); ++get) {
		auto audio = m_audios[get];
		if (!audioValid(audio)) continue;
		update3DAudio(audio, pos, vel);
		m_audios[put++] = audio;
	}
	m_audios.resize(put);
}

} // namespace sgd
