#pragma once
#include "Structs.h"
#include "Entity/Entity.h"

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

class Camera{
public:
	Camera(Vector3 pos, Vector3 dir, unsigned int fov, float aspectRatio);
	void Update();
	void updateConstantBuffer();

	unsigned int id;
	unsigned int gFieldOfView;
	float gAspectRatio;

	// Game Engine
	void Move(Vector3 moveDir, bool moveFast);
	void Rotate(float x, float y);

	// Movement factors
	bool wasMovingFast;
	const float initialMovementSpeed = 0.15f;
	const float initialRotationSpeed = 0.005f;
	float currentMovementSpeed = initialMovementSpeed;
	float currentRotationSpeed = initialRotationSpeed;
	const float fastMovementFactor = 1.02f;
	const float zoomFactor = 1.02f;

	// Entity following functionality
	void followEntity(Entity* followEntity, Vector3 entityCenterOffset, Vector3 followOffset);
	void Zoom(int zoomDirection);
	Entity* followedEntity = NULL;

	const float minDistanceToEntity = 5.0f;
	const float maxDistanceToEntity = 20.0f;
	Vector3 followEntityOffset = Vector3();
	Vector3 entityCenterOffset = Vector3();
	bool isFollowingEntity = false;

	// GPU side
	CameraVSConstantBuffer gCameraVSConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pCameraVSConstantBuffer;
	bool shouldUpdateGPUData = false;
	bool dataChanged = false;

	Vector3 rotation;
	Vector3 camPosition;
	Vector3 camLookAt;
	Vector3 lookDirection;
	Vector3 initialLookingDirection;
};