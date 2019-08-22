#pragma once
#include "Structs.h"

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;
namespace wrl = Microsoft::WRL;

class PointLight {
public:
	unsigned int id;

	PointLight(Vector3 position, float intensity);

	// Graphics
	dx::XMFLOAT3 gPosition;
	float gIntensity;

	PointLightConstantBuffer gLightConstBuffer;
	wrl::ComPtr<ID3D11Buffer> pLightConstantBuffer;
};