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
	bool shouldUpdateGPUData = false;
	bool hasChanged = false;

	// Graphics
	dx::XMFLOAT4 gDiffuseColor;
	float gIntensity;
	dx::XMFLOAT3 gPosition;
	dx::XMFLOAT3 gDirection;
	float gHalfSpotAngle;
};