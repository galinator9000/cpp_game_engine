#pragma once
#include "Structs.h"

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;
namespace wrl = Microsoft::WRL;

enum LIGHT_TYPE {
	DIRECTIONAL
};

class DirectionalLight {
public:
	unsigned int id;
	unsigned int type;

	DirectionalLight(Vector3 direction, float intensity);

	// Graphics
	dx::XMFLOAT3 gDirection;
	float gIntensity;

	LightConstantBuffer gLightConstBuffer;
	wrl::ComPtr<ID3D11Buffer> pLightConstantBuffer;
};