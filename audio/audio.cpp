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

	g_soloud = new SoLoud::Soloud();
	auto r = g_soloud->init(SoLoud::Soloud::LEFT_HANDED_3D);
	if(r!=SoLoud::SO_NO_ERROR) {
		SGD_LOG << "Soloud::init failed:"<<r;
		delete g_soloud;
		g_soloud = nullptr;
		return;
	}

	SGD_LOG << "Soloud::init success";

	appSuspending.connect(nullptr, [] {
		g_soloud->setPauseAll(true);
	});

	appResuming.connect(nullptr, [] {
		g_soloud->setPauseAll(false);
	});

	appExiting.connect(nullptr, []{
		g_soloud->deinit();
		delete g_soloud;
		g_soloud = nullptr;
	});
});

} // namespace

Sound::~Sound() {
	delete wav;
}

Expected<Sound*, FileioEx> loadSound(CPath path) {
	if(!g_soloud) return nullptr;

	auto data = loadData(path);
	if (!data) return data.error();

	auto wav = new SoLoud::Wav();
	if (wav->loadMem(data.result().data(), data.result().size(), true, false)) {
		return FileioEx("SoLoud failed to load wav data at \"" + path.str() + "\"");
	}

	return new Sound(wav);
}

uint32_t playSound(Sound* sound) {
	if(!g_soloud) return 0;
	
	return g_soloud->play(*sound->wav, 1.0f, 0.0f, false, 0);
}

uint32_t cueSound(Sound* sound) {
	if(!g_soloud) return 0;

	return g_soloud->play(*sound->wav, 1.0f, 0.0f, true, 0);
}

uint32_t playMusic(CPath path) {
	if(!g_soloud) return 0;

	if (g_musicChannel) {
		g_soloud->stop(g_musicChannel); // Maybe do this after loading?
		g_musicChannel = 0;
	}

	auto data = loadData(path);
	if (!data) {
		SGD_LOG << "Failed to load music data:"<<data.error().message();
		return 0;
	}

	auto r = g_musicStream.loadMem(data.result().data(), data.result().size(), true, false);
	if (r != SoLoud::SO_NO_ERROR) return 0;

	return g_musicChannel = g_soloud->play(g_musicStream);
}

void setAudioVolume(uint32_t audio, float volume) {
	if(!g_soloud) return;

	g_soloud->setVolume(audio, volume);
}

float audioVolume(uint32_t audio) {
	if(!g_soloud) return 0;

	return g_soloud->getVolume(audio);
}

void setAudioPan(uint32_t audio, float pan) {
	if(!g_soloud) return;

	g_soloud->setPan(audio, pan);
}

float audioPan(uint32_t audio) {
	if(!g_soloud) return 0;

	return g_soloud->getPan(audio);
}

void setAudioSampleRate(uint32_t audio, float rate) {
	if(!g_soloud) return;

	g_soloud->setSamplerate(audio, rate);
}

float audioSampleRate(uint32_t audio) {
	if(!g_soloud) return 0;

	return g_soloud->getSamplerate(audio);
}

void setAudioPitchScale(uint32_t audio, float scale) {
	if(!g_soloud) return;

	g_soloud->setRelativePlaySpeed(audio, scale);
}

float audioPitchScale(uint32_t audio) {
	if(!g_soloud) return 0;

	return g_soloud->getRelativePlaySpeed(audio);
}

void setAudioLooping(uint32_t audio, bool looping) {
	if(!g_soloud) return;

	g_soloud->setLooping(audio, looping);
}

bool audioLooping(uint32_t audio) {
	if(!g_soloud) return false;

	return g_soloud->getLooping(audio);
}

void setAudioPaused(uint32_t audio, bool paused) {
	if(!g_soloud) return;

	g_soloud->setPause(audio, paused);
}

bool audioPaused(uint32_t audio) {
	if(!g_soloud) return false;

	return g_soloud->getPause(audio);
}

bool audioValid(uint32_t audio) {
	if(!g_soloud) return false;

	return g_soloud->isValidVoiceHandle(audio);
}

void stopAudio(uint32_t audio) {
	if(!g_soloud) return;

	g_soloud->stop(audio);
}

} // namespace sgd
