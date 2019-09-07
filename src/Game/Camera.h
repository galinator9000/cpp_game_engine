#pragma once
#include "Structs.h"
#include "Entity/Entity.h"

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

class Camera{
public:
	Camera(Vector3 pos, unsigned int fov, float aspectRatio, Entity* followEntity=NULL, Vector3 followOffset=Vector3(0,0,0));
	void Update();
	void updateConstantBuffer();

	unsigned int id;
	unsigned int gFieldOfView;
	float gAspectRatio;

	// Game Engine
	void Move(Vector3 moveDir, bool moveFast);
	void Rotate(float x, float y);

	// Movement factors
	const float fastMovementFactor = 1.02f;
	bool wasMovingFast;
	const float initialMovementSpeed = 0.15f;
	const float initialRotationSpeed = 0.005f;
	float currentMovementSpeed = initialMovementSpeed;
	float currentRotationSpeed = initialRotationSpeed;

	// Entity following functionality
	void followEntity(Entity* followEntity, Vector3 followOffset);
	void scaleFollowOffset(Vector3 scaling);
	Entity* followedEntity = NULL;
	Vector3 followOffset = Vector3();
	bool isFollowingEntity = false;

	// GPU side
	CameraVSConstantBuffer gCameraVSConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pCameraVSConstantBuffer;
	bool shouldUpdateGPUData = false;
	bool hasChanged = false;

	Vector3 rotation;
	dx::XMFLOAT3 camPosition;
	dx::XMFLOAT3 camLookAt;
	dx::XMFLOAT3 lookDirection;
};