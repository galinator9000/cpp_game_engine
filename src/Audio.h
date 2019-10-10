#pragma once
#include "al.h"
#include "alc.h"
#pragma comment(lib, "OpenAL32.lib")
#include <wrl.h>
#include <Windows.h>

class Audio{
public:
	Audio();

private:
	ALCdevice* pDevice;
	ALCcontext* pDeviceContext;
};