#include "audiospace.h"

#include <audio/exports.h>

namespace sgd {

void AudioSpace::update() {
	for (AudioListener* l : m_listeners) {
		l->update();
		break;	// Only one listener supported by SoLoud I think...
	}
	for (AudioEmitter* e : m_emitters) {
		e->update();
	}
	update3DAudio();
}

void AudioSpace::add(AudioListener* listener) {
	SGD_ASSERT(!contains(m_listeners, listener));
	m_listeners.emplace_back(listener);
}

void AudioSpace::remove(AudioListener* listener) {
	SGD_ASSERT(contains(m_listeners, listener));
	sgd::remove(m_listeners, listener);
}

void AudioSpace::add(AudioEmitter* emitter) {
	SGD_ASSERT(!contains(m_emitters, emitter));
	m_emitters.emplace_back(emitter);
}

void AudioSpace::remove(AudioEmitter* emitter) {
	SGD_ASSERT(contains(m_emitters, emitter));
	sgd::remove(m_emitters, emitter);
}

} // namespace sgd
