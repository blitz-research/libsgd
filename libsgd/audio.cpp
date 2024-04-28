#include "internal.h"

SGD_Sound SGD_DECL sgd_LoadSound(SGD_String path) {
	sgdx::started();
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

void SGD_DECL sgd_SetAudioVolume(int audio, float volume) {
	sgdx::started();

	sgd::setAudioVolume(audio, volume);
}

void SGD_DECL sgd_SetAudioPan(int audio, float pan) {
	sgdx::started();

	sgd::setAudioPan(audio, pan);
}

void SGD_DECL sgd_SetAudioPitchScale(uint32_t audio, float scale) {
	sgdx::started();

	sgd::setAudioPitchScale(audio, scale);
}

void SGD_DECL sgd_SetAudioLooping(uint32_t audio, SGD_Bool looping) {
	sgdx::started();

	sgd::setAudioLooping(audio, looping);
}

void SGD_DECL sgd_SetAudioPaused(uint32_t audio, SGD_Bool paused) {
	sgdx::started();

	sgd::setAudioPaused(audio, paused);
}

SGD_Bool SGD_DECL sgd_AudioValid(uint32_t audio) {
	sgdx::started();

	return sgd::audioValid(audio);
}

void SGD_DECL sgd_StopAudio(uint32_t audio) {
	sgdx::started();

	sgd::stopAudio(audio);
}
