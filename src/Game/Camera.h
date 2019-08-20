#pragma once
#include "Structs.h"

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

class Camera{
public:
	Camera(Vector3 pos, unsigned int fov, float aspectRatio);
	void Update(bool initial=false);
	void updateConstantBuffer();

	unsigned int id;
	unsigned int gFieldOfView;
	float gAspectRatio;

	// Game Engine
	void Move(Vector3 moveDir, bool moveFast);
	void Rotate(float x, float y);

	const float fastMovementFactor = 1.02f;
	bool wasMovingFast;

	const float initialMovementSpeed = 0.15f;
	const float initialRotationSpeed = 0.005f;
	float currentMovementSpeed = initialMovementSpeed;
	float currentRotationSpeed = initialRotationSpeed;

	// GPU side
	ViewProjectionMatrices gViewProjection;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pViewProjectionBuffer;
	bool shouldUpdateData = false;
	bool hasChanged = false;

	dx::XMFLOAT3 lookDirection;
	dx::XMFLOAT3 rotation;
	dx::XMFLOAT3 camPosition;
	dx::XMFLOAT3 camLookAt;
};