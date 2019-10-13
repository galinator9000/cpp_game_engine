#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "PxPhysicsAPI.h"
#include <wrl.h>
#include <vector>
#include <algorithm>

#include "Structs.h"

using namespace physx;
namespace dx = DirectX;
namespace wrl = Microsoft::WRL;

class TextureSampler{
public:
	TextureSampler(){}
	unsigned int id;

	wrl::ComPtr<ID3D11SamplerState> pSamplerState;
};