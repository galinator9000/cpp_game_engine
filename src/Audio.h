#pragma once
#include "xaudio2.h"
#include <Windows.h>
#include <wrl.h>

#include "Config.h"

class Audio{
public:
	Audio();

private:
	HRESULT hr;

	Microsoft::WRL::ComPtr<IXAudio2> pXAudio2;
};