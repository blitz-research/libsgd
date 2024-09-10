#pragma once

#include "audiolistener.h"
#include "audioemitter.h"

namespace sgd {

SGD_SHARED(AudioListener);
SGD_SHARED(AudioEmitter);

struct AudioSpace : Shared {
	SGD_OBJECT_TYPE(AudioSpace, Shared);

	void update();

private:
	friend struct AudioListener;
	friend struct AudioEmitter;

	Vector<AudioListenerPtr> m_listeners;
	Vector<AudioEmitterPtr> m_emitters;

	void add(AudioListener* listener);
	void remove(AudioListener* listener);

	void add(AudioEmitter* emitter);
	void remove(AudioEmitter* emitter);
};


}
