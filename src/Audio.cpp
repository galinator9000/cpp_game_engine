#include "Audio.h"

Audio::Audio() {
	this->pDevice = alcOpenDevice(0);
	this->pDeviceContext = alcCreateContext(this->pDevice, NULL);
	alcMakeContextCurrent(this->pDeviceContext);

	// Create buffer.
	alGetError();

	unsigned int* audioBuffer = new unsigned int[4096];
	alGenBuffers(16, audioBuffer);

	if (alGetError() != AL_NO_ERROR) {
		OutputDebugStringA("alGenBuffers error");
		return;
	}
}