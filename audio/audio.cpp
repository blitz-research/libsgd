#include "audio.h"

#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_wavstream.h"

namespace sgd {

namespace {

SoLoud::Soloud* g_soloud;

void init() {
	if (g_soloud) return;
	g_soloud = new SoLoud::Soloud();
	g_soloud->init();

	appSuspended.connect(nullptr, [] {
		if (g_soloud) g_soloud->setPauseAll(true);
	});

	appResumed.connect(nullptr, [] {
		if (g_soloud) g_soloud->setPauseAll(false);
	});
}

SoLoud::Soloud* soloud() {
	if (!g_soloud) init();
	return g_soloud;
}

} // namespace

Sound::~Sound() {
	delete wav;
}

Expected<Sound*, FileioEx> loadSound(CPath path) {
	init();

	auto data = loadData(path);
	if (!data) return data.error();

	auto wav = new SoLoud::Wav();
	if (wav->loadMem(data.result().data(), data.result().size(), true, false)) {
		return FileioEx("SoLoud failed to load wav data at \"" + path.str() + "\"");
	}

	return new Sound(wav);
}

uint32_t playSound(Sound* sound) {
	return soloud()->play(*sound->wav, 1.0f, 0.0f, false, 0);
}

uint32_t cueSound(Sound* sound) {
	return soloud()->play(*sound->wav, 1.0f, 0.0f, true, 0);
}

void setAudioVolume(uint32_t audio, float volume) {
	soloud()->setVolume(audio, volume);
}

float audioVolume(uint32_t audio) {
	return soloud()->getVolume(audio);
}

void setAudioPan(uint32_t audio, float pan) {
	soloud()->setPan(audio, pan);
}

float audioPan(uint32_t audio) {
	return soloud()->getPan(audio);
}

void setAudioSampleRate(uint32_t audio, float rate) {
	soloud()->setSamplerate(audio, rate);
}

void setAudioPitchScale(uint32_t audio, float scale) {
	soloud()->setRelativePlaySpeed(audio, scale);
}

float audioPitchScale(uint32_t audio) {
	return soloud()->getRelativePlaySpeed(audio);
}

void setAudioLooping(uint32_t audio, bool looping) {
	soloud()->setLooping(audio, looping);
}

bool audioLooping(uint32_t audio) {
	return soloud()->getLooping(audio);
}

void setAudioPaused(uint32_t audio, bool paused) {
	soloud()->setPause(audio, paused);
}

bool audioPaused(uint32_t audio) {
	return soloud()->getPause(audio);
}

bool audioValid(uint32_t audio) {
	return soloud()->isValidVoiceHandle(audio);
}

void stopAudio(uint32_t audio) {
	soloud()->stop(audio);
}

uint32_t playMusic(CPath path) {
	init();

	SGD_ASSERT(path.isFilePath());

	auto wavStream = new SoLoud::WavStream();
	SGD_LOG << "### loadFile:" << wavStream->load(path.filePath().u8string().c_str());

	soloud()->play(*wavStream);

	return 0;
}

} // namespace sgd
