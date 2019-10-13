#pragma once
#include "Structs.h"

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;
namespace wrl = Microsoft::WRL;

const enum LIGHT_TYPE {
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};

class Light {
public:
	unsigned int id;
	unsigned int type;

	// Functions for all light types.
	void setIntensity(float intensity);
	void setColor(Color color);
	void setDirection(Vector3 newDirection);
	void setPosition(Vector3 newPosition);

	void Update();
	void Reset();
	void updateConstantBuffer();
	bool shouldUpdateGPUData = true;
	bool dataChanged = false;

	// Graphics
	dx::XMFLOAT4 gDiffuseColor = dx::XMFLOAT4(1,1,1,1);
	float gIntensity = 0;
	dx::XMFLOAT3 gPosition = dx::XMFLOAT3(0,0,0);
	dx::XMFLOAT3 gDirection = dx::XMFLOAT3(0,1,0);
	float gHalfSpotAngle = 0;
};