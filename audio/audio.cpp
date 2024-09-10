#include "audio.h"

#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_wavstream.h"

namespace sgd {

namespace {

SoLoud::Soloud* g_soloud;

SoLoud::WavStream g_musicStream;
uint32_t g_musicChannel;

auto initing = appIniting.connect(nullptr, [] {
	SGD_LOG << "Soloud initializing";

	g_soloud = new SoLoud::Soloud();
	auto r = g_soloud->init(SoLoud::Soloud::LEFT_HANDED_3D);
	if (r != SoLoud::SO_NO_ERROR) {
		SGD_LOG << "Soloud failed to initialize, return code" << r;
		SGD_LOG << "Audio features will be unavalable";
		delete g_soloud;
		g_soloud = nullptr;
		return;
	}

	appSuspending.connect(nullptr, [] { //
		g_soloud->setPauseAll(true);
	});

	appResuming.connect(nullptr, [] { //
		g_soloud->setPauseAll(false);
	});

	appExiting.connect(nullptr, [] {
		SGD_LOG << "Soloud deinitializing";
		g_soloud->deinit();
		delete g_soloud;
		g_soloud = nullptr;
	});
});

} // namespace

Expected<Sound*, FileioEx> loadSound(CPath path) {
	if (!g_soloud) return nullptr;

	auto data = loadData(path);
	if (!data) return data.error();

	auto wav= new SoLoud::Wav();
	if (wav->loadMem(data.result().data(), data.result().size(), true, false)) {
		delete wav;
		return SGD_FILEIOEX("SoLoud failed to load wav data at \"" + path.str() + "\"");
	}

	return new Sound(wav);
}

uint32_t playSound(Sound* sound) {
	if (!g_soloud) return 0;

	return g_soloud->play(*sound->wav, 1.0f, 0.0f, false, 0);
}

uint32_t cueSound(Sound* sound) {
	if (!g_soloud) return 0;

	return g_soloud->play(*sound->wav, 1.0f, 0.0f, true, 0);
}

uint32_t playMusic(CPath path) {
	if (!g_soloud) return 0;

	if (g_musicChannel) {
		g_soloud->stop(g_musicChannel); // Maybe do this after loading?
		g_musicChannel = 0;
	}

	auto data = loadData(path);
	if (!data) {
		SGD_LOG << "Failed to load music data:" << data.error().message();
		return 0;
	}

	auto r = g_musicStream.loadMem(data.result().data(), data.result().size(), true, false);
	if (r != SoLoud::SO_NO_ERROR) return 0;

	return g_musicChannel = g_soloud->play(g_musicStream);
}

void setAudioVolume(uint32_t audio, float volume) {
	if (!g_soloud) return;

	g_soloud->setVolume(audio, volume);
}

float audioVolume(uint32_t audio) {
	if (!g_soloud) return 0;

	return g_soloud->getVolume(audio);
}

void setAudioPan(uint32_t audio, float pan) {
	if (!g_soloud) return;

	g_soloud->setPan(audio, pan);
}

float audioPan(uint32_t audio) {
	if (!g_soloud) return 0;

	return g_soloud->getPan(audio);
}

void setAudioSampleRate(uint32_t audio, float rate) {
	if (!g_soloud) return;

	g_soloud->setSamplerate(audio, rate);
}

float audioSampleRate(uint32_t audio) {
	if (!g_soloud) return 0;

	return g_soloud->getSamplerate(audio);
}

void setAudioPitchScale(uint32_t audio, float scale) {
	if (!g_soloud) return;

	g_soloud->setRelativePlaySpeed(audio, scale);
}

float audioPitchScale(uint32_t audio) {
	if (!g_soloud) return 0;

	return g_soloud->getRelativePlaySpeed(audio);
}

void setAudioLooping(uint32_t audio, bool looping) {
	if (!g_soloud) return;

	g_soloud->setLooping(audio, looping);
}

bool audioLooping(uint32_t audio) {
	if (!g_soloud) return false;

	return g_soloud->getLooping(audio);
}

void setAudioPaused(uint32_t audio, bool paused) {
	if (!g_soloud) return;

	g_soloud->setPause(audio, paused);
}

bool audioPaused(uint32_t audio) {
	if (!g_soloud) return false;

	return g_soloud->getPause(audio);
}

bool audioValid(uint32_t audio) {
	if (!g_soloud) return false;

	return g_soloud->isValidVoiceHandle(audio);
}

void stopAudio(uint32_t audio) {
	if (!g_soloud) return;

	g_soloud->stop(audio);
}

// ***** 3D Audio *****

namespace {

float rolloffFactor = 1.0f;
float dopplerFactor = 1.0f;
float distanceFactor = 1.0f;

} // namespace

void set3DAudioConfig(float roll, float dopp, float dist) {
	if (!g_soloud) return;

	rolloffFactor = roll;
	dopplerFactor = dopp;
	distanceFactor = dist;
}

void update3DAudioListener(CVec3f pos, CVec3f fwd, CVec3f up, CVec3f vel) {
	if (!g_soloud) return;

	auto sc = distanceFactor;
	g_soloud->set3dListenerParameters(pos.x * sc, pos.y * sc, pos.z * sc, fwd.x, fwd.y, fwd.z, up.x, up.y, up.z, vel.x * sc,
									  vel.y * sc, vel.z * sc);
}

uint32_t play3DSound(Sound* sound, CVec3f pos, CVec3f vel) {
	if (!g_soloud) return 0;

	sound->wav->set3dAttenuation(SoLoud::AudioSource::LINEAR_DISTANCE, rolloffFactor);
	sound->wav->set3dDopplerFactor(dopplerFactor);

	auto sc = distanceFactor;
	return g_soloud->play3d(*sound->wav, pos.x * sc, pos.y * sc, pos.z *sc, vel.x *sc, vel.y * sc, vel.z * sc, 1.0f, false, 0);
}

void update3DAudio(uint32_t audio, CVec3f pos, CVec3f vel) {
	if (!g_soloud) return;

	auto sc = distanceFactor;
	g_soloud->set3dSourceParameters(audio, pos.x * sc, pos.y * sc, pos.z * sc, vel.x * sc, vel.y * sc, vel.z * sc);
}

void update3DAudio() {
	if (!g_soloud) return;

	g_soloud->update3dAudio();
}

} // namespace sgd
