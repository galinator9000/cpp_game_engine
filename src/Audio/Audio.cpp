#include "Audio.h"

Audio::Audio() {
	this->hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	this->hr = XAudio2Create(
		&this->pXAudio2,
		XAUDIO2_DEBUG_ENGINE,
		XAUDIO2_DEFAULT_PROCESSOR
	);
}