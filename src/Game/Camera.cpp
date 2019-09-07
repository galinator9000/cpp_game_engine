#include "Camera.h"

Camera::Camera(Vector3 pos, unsigned int fov, float aspectRatio, Entity* followEntity, Vector3 followOffset){
	this->gFieldOfView = fov;
	this->gAspectRatio = aspectRatio;

	this->rotation = dx::XMFLOAT3(0, 0, 0);

	// Build looking direction vector
	dx::XMStoreFloat3(
		&this->lookDirection,
		dx::XMVector3Transform(
			dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
			dx::XMMatrixRotationRollPitchYaw(
				this->rotation.x,
				this->rotation.y,
				0
			)
		)
	);

	// Fill camera position and looking direction vector
	this->camPosition = dx::XMFLOAT3(pos.x, pos.y, pos.z);
	this->camLookAt = dx::XMFLOAT3(
		this->camPosition.x + this->lookDirection.x,
		this->camPosition.y + this->lookDirection.y,
		this->camPosition.z + this->lookDirection.z
	);

	// If entity is provided to follow, set it.
	if (followEntity != NULL) {
		this->followEntity(followEntity, followOffset);
	}

	this->updateConstantBuffer();
}

void Camera::Update() {
	this->shouldUpdateGPUData = false;

	if (this->isFollowingEntity || this->hasChanged) {
		this->hasChanged = false;
		this->updateConstantBuffer();
		this->shouldUpdateGPUData = true;
	}
}

void Camera::updateConstantBuffer() {
	// Update constant buffer that held on class.
	if (!isFollowingEntity) {
		// Update struct.
		// Position of the camera.
		this->gCameraVSConstantBuffer.cameraPosition.x = this->camPosition.x;
		this->gCameraVSConstantBuffer.cameraPosition.y = this->camPosition.y;
		this->gCameraVSConstantBuffer.cameraPosition.z = this->camPosition.z;
	}
	// Following entity.
	else{
		// Set camera's position to (entity's position + follow offset).
		this->camPosition.x = this->followedEntity->gPosition.x + this->followOffset.x;
		this->camPosition.y = this->followedEntity->gPosition.y + this->followOffset.y;
		this->camPosition.z = this->followedEntity->gPosition.z + this->followOffset.z;

		// Set looking direction to entity's position.
		this->camLookAt.x = this->followedEntity->gPosition.x;
		this->camLookAt.y = this->followedEntity->gPosition.y;
		this->camLookAt.z = this->followedEntity->gPosition.z;

		// Position of the camera.
		this->gCameraVSConstantBuffer.cameraPosition.x = this->camPosition.x;
		this->gCameraVSConstantBuffer.cameraPosition.y = this->camPosition.y;
		this->gCameraVSConstantBuffer.cameraPosition.z = this->camPosition.z;
	}

	// Update View matrix.
	dx::XMStoreFloat4x4(
		&this->gCameraVSConstantBuffer.viewMatrix,
		dx::XMMatrixTranspose(
			dx::XMMatrixLookAtLH(
				dx::XMLoadFloat3(&this->camPosition),
				dx::XMLoadFloat3(&this->camLookAt),
				dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
			)
		)
	);

	// Update Projection matrix.
	dx::XMStoreFloat4x4(
		&this->gCameraVSConstantBuffer.projectionMatrix,
		dx::XMMatrixTranspose(
			dx::XMMatrixPerspectiveFovLH(
			((float)this->gFieldOfView / 360) * (2.0f * dx::XM_PI),
				this->gAspectRatio,
				0.5f,
				100.0f
			)
		)
	);
}

void Camera::Move(Vector3 moveDir, bool moveFast) {
	// Move camera if it isn't following any entity.
	if (!this->isFollowingEntity) {
		this->moveCamera(moveDir, moveFast);
	}
}

void Camera::Rotate(float yaw, float pitch) {
	// Rotate camera around it's local axis.
	if (!this->isFollowingEntity) {
		this->rotateCamera(yaw, pitch);
	}
	// Rotate around followed entity.
	else {
		this->rotateCameraEntity(yaw, pitch);
	}
}

// Local transformations
void Camera::moveCamera(Vector3 moveDir, bool moveFast) {
	if (this->wasMovingFast) {
		this->currentMovementSpeed *= fastMovementFactor;
	}
	else {
		this->currentMovementSpeed = this->initialMovementSpeed;
	}

	// Calculate camera movement direction.
	dx::XMFLOAT3 camTranslation = dx::XMFLOAT3(moveDir.x, moveDir.y, moveDir.z);
	dx::XMStoreFloat3(
		&camTranslation,
		dx::XMVector3Transform(
			dx::XMLoadFloat3(&camTranslation),
			(
				dx::XMMatrixRotationRollPitchYaw(
					this->rotation.x,
					this->rotation.y,
					0
				) *
				dx::XMMatrixScaling(currentMovementSpeed, currentMovementSpeed, currentMovementSpeed)
			)
		)
	);

	// Update camera position
	this->camPosition.x += camTranslation.x;
	this->camPosition.y += camTranslation.y;
	this->camPosition.z += camTranslation.z;

	// Update looking direction vector
	this->camLookAt = dx::XMFLOAT3(
		this->camPosition.x + this->lookDirection.x,
		this->camPosition.y + this->lookDirection.y,
		this->camPosition.z + this->lookDirection.z
	);

	if (moveFast) {
		this->wasMovingFast = true;
	}
	else {
		this->wasMovingFast = false;
	}
	this->hasChanged = true;
}
void Camera::rotateCamera(float yaw, float pitch) {
	yaw *= this->currentRotationSpeed;
	pitch *= this->currentRotationSpeed;

	// Update camera rotation
	this->rotation.x += pitch;
	this->rotation.y += yaw;

	// Clip Pitch (Rotation on X axis) value to
	// -PI/2 and PI/2 in radians
	// -90 and 90 in degrees
	if (this->rotation.x >= dx::XM_PI / 2) {
		this->rotation.x = dx::XM_PI * 0.4999f;
	}
	else if (this->rotation.x <= -(dx::XM_PI / 2)) {
		this->rotation.x = -(dx::XM_PI * 0.4999f);
	}

	// Update looking direction vector
	dx::XMStoreFloat3(
		&this->lookDirection,
		dx::XMVector3Transform(
			dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
			dx::XMMatrixRotationRollPitchYaw(
				this->rotation.x,
				this->rotation.y,
				0
			)
		)
	);

	// Update looking direction vector
	this->camLookAt = dx::XMFLOAT3(
		this->camPosition.x + this->lookDirection.x,
		this->camPosition.y + this->lookDirection.y,
		this->camPosition.z + this->lookDirection.z
	);

	this->hasChanged = true;
}

// Rotate around followed entity.
void Camera::rotateCameraEntity(float yaw, float pitch) {
	yaw *= this->currentRotationSpeed;
	pitch *= this->currentRotationSpeed;

	// Update camera rotation
	this->rotation.x += pitch;
	this->rotation.y += yaw;

	// Clip Pitch (Rotation on X axis) value to
	// -PI/2 and PI/2 in radians
	// -90 and 90 in degrees
	if (this->rotation.x >= dx::XM_PI / 2) {
		this->rotation.x = dx::XM_PI * 0.4999f;
	}
	else if (this->rotation.x <= -(dx::XM_PI / 2)) {
		this->rotation.x = -(dx::XM_PI * 0.4999f);
	}

	// Update looking direction vector
	dx::XMStoreFloat3(
		&this->lookDirection,
		dx::XMVector3Transform(
			dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
			dx::XMMatrixRotationRollPitchYaw(
				this->rotation.x,
				this->rotation.y,
				0
			)
		)
	);

	// Update looking direction vector
	this->camLookAt = dx::XMFLOAT3(
		this->camPosition.x + this->lookDirection.x,
		this->camPosition.y + this->lookDirection.y,
		this->camPosition.z + this->lookDirection.z
	);

	this->hasChanged = true;
}

void Camera::followEntity(Entity* followEntity, Vector3 followOffset) {
	this->followedEntity = followEntity;
	this->followOffset = followOffset;
	this->isFollowingEntity = true;

	// Set values to initial values.
	float currentMovementSpeed = initialMovementSpeed;
	float currentRotationSpeed = initialRotationSpeed;
}