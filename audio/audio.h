#pragma once

#include "app/exports.h"

namespace SoLoud {
struct Wav;
}

namespace sgd {

SGD_SHARED(Sound);

struct Sound : Shared {
	SGD_OBJECT_TYPE(Sound, Shared);

	SoLoud::Wav* const wav;

	Property<bool> looping;

	explicit Sound(SoLoud::Wav* wav) : wav(wav) {
	}
};

Expected<Sound*, FileioEx> loadSound(CPath path);

uint32_t playSound(Sound* sound);

uint32_t cueSound(Sound* sound);

uint32_t playMusic(CPath path);

void setAudioVolume(uint32_t audio, float pan);
float audioVolume(uint32_t audio);

void setAudioPan(uint32_t audio, float pan);
float audioPan(uint32_t audio);

void setAudioSampleRate(uint32_t audio, float rate);
float audioSampleRate(uint32_t audio);

void setAudioPitchScale(uint32_t audio, float scale);
float audioPitchScale(uint32_t audio);

void setAudioLooping(uint32_t audio, bool looping);
bool audioLooping(uint32_t audio);

void setAudioPaused(uint32_t audio, bool paused);
bool audioPaused(uint32_t audio);

bool audioValid(uint32_t audio);
void stopAudio(uint32_t audio);

// ***** 3D Audio *****

void set3DAudioConfig(float roll, float dopp, float dist);
void update3DAudioListener(CVec3f pos, CVec3f fwd, CVec3f up, CVec3f vel);

uint32_t play3DSound(Sound* sound, CVec3f pos, CVec3f vel);

void update3DAudio(uint32_t audio, CVec3f pos, CVec3f vel);

void update3DAudio();

} // namespace sgd
