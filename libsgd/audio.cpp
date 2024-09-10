#include "internal.h"

SGD_Sound SGD_DECL sgd_LoadSound(SGD_String path) {
	sgdx::started();
	auto sound = sgd::loadSound(sgd::Path(path));
	if (!sound) sgdx::error("Failed to load sound", sound.error());
	return sgdx::createHandle((sgd::Shared*)sound.result());
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

void SGD_DECL sgd_SetAudioPitchScale(int audio, float scale) {
	sgdx::started();
	sgd::setAudioPitchScale(audio, scale);
}

void SGD_DECL sgd_SetAudioLooping(int audio, SGD_Bool looping) {
	sgdx::started();
	sgd::setAudioLooping(audio, looping);
}

void SGD_DECL sgd_SetAudioPaused(int audio, SGD_Bool paused) {
	sgdx::started();
	sgd::setAudioPaused(audio, paused);
}

SGD_Bool SGD_DECL sgd_IsAudioValid(int audio) {
	sgdx::started();
	return sgd::audioValid(audio);
}

void SGD_DECL sgd_StopAudio(int audio) {
	sgdx::started();
	sgd::stopAudio(audio);
}

void SGD_DECL sgd_Set3DAudioConfig(float rolloff, float dopplerScale, float distanceScale) {
	sgd::set3DAudioConfig(rolloff, dopplerScale, distanceScale);
}

SGD_AudioListener SGD_DECL sgd_Create3DAudioListener(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	auto listener = new sgd::AudioListener(entity);
	return sgdx::createHandle(listener);
}

int SGD_DECL sgd_Play3DSound(SGD_Sound hsound, SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	auto sound = sgdx::resolveHandle<sgd::Sound>(hsound);
	auto emitter = sgd::AudioEmitter::getOrCreate(entity);
	return (int)emitter->play3DSound(sound);
}

void SGD_DECL sgd_Update3DAudio() {
	sgdx::mainScene()->audioSpace()->update();
}
