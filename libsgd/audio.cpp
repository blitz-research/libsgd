#include "internal.h"

SGD_Sound SGD_DECL sgd_LoadSound(SGD_String path) {
	auto sound = sgd::loadSound(sgd::Path(path));
	if (!sound) sgdx::error("Failed to load sound", sound.error());

	return sgdx::createHandle(sound.result());
}

int SGD_DECL sgd_PlaySound(SGD_Sound hsound) {
	auto sound = sgdx::resolveHandle<sgd::Sound>(hsound);

	return (int)sgd::playSound(sound);
}

int SGD_DECL sgd_CueSound(SGD_Sound hsound) {
	auto sound = sgdx::resolveHandle<sgd::Sound>(hsound);

	return (int)sgd::cueSound(sound);
}

void SGD_DECL sgd_SetAudioPaused(int audio, SGD_Bool paused) {
	sgd::setAudioPaused(audio, paused);

}

void SGD_DECL sgd_SetAudioVolume(int audio, float volume) {
	sgd::setAudioVolume(audio, volume);
}

void SGD_DECL sgd_SetAudioPan(int audio, float pan) {
	sgd::setAudioPan(audio, pan);
}
