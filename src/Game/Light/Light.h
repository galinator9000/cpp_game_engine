#pragma once
#include "Shadow/ShadowBox.h"
#include "Structs.h"

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;
namespace wrl = Microsoft::WRL;

class Light {
public:
	LIGHT_TYPE type;
	bool isActive = false;
	unsigned int id;
	bool isCastingShadow = false;

	Color gColor = Color(1, 1, 1, 1);
	float gIntensity = 0;
	Vector3 gPosition = Vector3(0, 0, 0);
	Vector3 gDirection = Vector3(0, 1, 0);
	float gHalfSpotAngle = 0;

	// Functions for all light types.
	void setIntensity(float intensity);
	void setColor(Color color);
	void setDirection(Vector3 newDirection);
	void setPosition(Vector3 newPosition);

	ShadowBox* gShadowBox;

	void Update();
	void Reset();
	void updateConstantBuffer();
	bool shouldUpdateGPUData = true;
	bool dataChanged = false;
};