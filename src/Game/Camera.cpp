#include "Camera.h"

Camera::Camera(Vector3 position, unsigned int fov, float aspectRatio, Entity* followEntity, Vector3 followOffset){
	this->gFieldOfView = fov;
	this->gAspectRatio = aspectRatio;

	this->rotation = Vector3();

	// Set looking direction vector to +Z axis
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
	this->camPosition = dx::XMFLOAT3(position.x, position.y, position.z);
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
	if (this->isFollowingEntity) {
		// Set looking direction to entity's position.
		this->camLookAt.x = this->followedEntity->gPosition.x;
		this->camLookAt.y = this->followedEntity->gPosition.y;
		this->camLookAt.z = this->followedEntity->gPosition.z;

		// Set camera's position to (entity's position + (follow offset * rotation)).
		dx::XMStoreFloat3(
			&this->camPosition,
			dx::XMVector3Transform(
				dx::XMVectorSet(this->followOffset.x, this->followOffset.y, this->followOffset.z, 0.0f),
				(
					// Rotate offset by rotation values.
					dx::XMMatrixRotationRollPitchYaw(
						this->rotation.x,
						this->rotation.y,
						0
					) *
					// Translate calculated position to world-space.
					dx::XMMatrixTranslation(
						this->followedEntity->gPosition.x,
						this->followedEntity->gPosition.y,
						this->followedEntity->gPosition.z
					)
				)
			)
		);
	}
	else {
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
		this->camLookAt = dx::XMFLOAT3(
			this->camPosition.x + this->lookDirection.x,
			this->camPosition.y + this->lookDirection.y,
			this->camPosition.z + this->lookDirection.z
		);
	}

	//// Update constant buffer on class which is provided to GPU side.
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

	// Provide position of the camera to buffer.
	this->gCameraVSConstantBuffer.cameraPosition.x = this->camPosition.x;
	this->gCameraVSConstantBuffer.cameraPosition.y = this->camPosition.y;
	this->gCameraVSConstantBuffer.cameraPosition.z = this->camPosition.z;
}

void Camera::Move(Vector3 translation, bool moveFast) {
	// Cameras that followes an entity, shouldn't be translated by hand.
	if (this->isFollowingEntity) {
		return;
	}

	if (moveFast && this->wasMovingFast) {
		this->currentMovementSpeed *= fastMovementFactor;
	}
	else {
		this->currentMovementSpeed = this->initialMovementSpeed;
	}

	// Calculate camera movement direction.
	dx::XMFLOAT3 camTranslation = dx::XMFLOAT3(translation.x, translation.y, translation.z);

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

	this->wasMovingFast = moveFast;
	this->hasChanged = true;
}

void Camera::Rotate(float xDelta, float yDelta) {
	xDelta *= this->currentRotationSpeed;
	yDelta *= this->currentRotationSpeed;

	// Update camera rotation
	// xDelta and yDelta comes from mouse's raw movement.
	// Mouse movement on +X axis (on screen space) corresponds to rotation on Y axis on our 3D world.
	if (this->isFollowingEntity) {
		// Rotation is inverted on X axis while following entity.
		this->rotation.x -= yDelta;
		this->rotation.y += xDelta;

		// Clip Pitch (Rotation on X axis) value to
		// -PI/2 and PI/2 in radians
		// -90 and 90 in degrees
		if (this->rotation.x >= dx::XM_PI / 2) {
			this->rotation.x = dx::XM_PI * 0.4999f;
		}
		else if (this->rotation.x <= -(dx::XM_PI / 2)) {
			this->rotation.x = -(dx::XM_PI * 0.4999f);
		}
	}
	else {
		this->rotation.x += yDelta;
		this->rotation.y += xDelta;

		// Clip Pitch (Rotation on X axis) value to
		// -PI/2 and PI/2 in radians
		// -90 and 90 in degrees
		if (this->rotation.x >= dx::XM_PI / 2) {
			this->rotation.x = dx::XM_PI * 0.4999f;
		}
		else if (this->rotation.x <= -(dx::XM_PI / 2)) {
			this->rotation.x = -(dx::XM_PI * 0.4999f);
		}
	}

	this->hasChanged = true;
}

// Entity following functionality.
void Camera::followEntity(Entity* followEntity, Vector3 followOffset) {
	this->followedEntity = followEntity;
	this->followOffset = followOffset;
	this->isFollowingEntity = true;

	// Set values to initial values.
	float currentMovementSpeed = initialMovementSpeed;
	float currentRotationSpeed = initialRotationSpeed;
	this->rotation = Vector3();
}

void Camera::scaleFollowOffset(Vector3 scaling) {
	this->followOffset = this->followOffset * scaling;
}