#pragma once
#include "Structs.h"
#include "Entity/Entity.h"

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

const enum PROJECTION_TYPE {
	PERSPECTIVE,
	ORTHOGRAPHIC
};

class Camera{
public:
	Camera(Vector3 pos, Vector3 dir, float WIDTH, float HEIGHT, PROJECTION_TYPE projectionType);
	void Update();
	void updateConstantBuffer();

	unsigned int id;
	PROJECTION_TYPE projectionType;

	// Game Engine
	void setPosition(Vector3 newPos);
	void setDirection(Vector3 newDir);
	void setPerspectiveProjection(float fieldOfView=90, float aspectRatio=4/3, float nearZ=0.5f, float farZ=100.0f);
	void setOrthographicProjection(float viewWidth, float viewHeight, float nearZ = 0.5f, float farZ = 100.0f);

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

	Vector3 gPosition;
	Vector3 gDirection;

	Vector3 rotation;
	Vector3 camLookAt;
};