#include "Camera.h"

Camera::Camera() {
	this->gPosition = Vector3();
	this->gDirection = Vector3();
	this->rotation = Vector3();
	this->setPerspectiveProjection();

	this->updateConstantBuffer();
}

Camera::Camera(Vector3 position, Vector3 direction, float WIDTH, float HEIGHT, PROJECTION_TYPE projectionType){
	this->gPosition = position;
	this->gDirection = direction.normalize();

	this->rotation = Vector3();
	this->gDirection.storeXMVECTOR(
		&dx::XMVector3Transform(
			dx::XMVectorSet(0, 0, 1, 0),
			dx::XMMatrixRotationRollPitchYaw(
				this->rotation.x,
				this->rotation.y,
				0
			)
		)
	);

	// Default perspective.
	switch (projectionType) {
		case PERSPECTIVE:
			this->setPerspectiveProjection();
			break;
		case ORTHOGRAPHIC:
			this->setOrthographicProjection(WIDTH, HEIGHT);
			break;
	}

	this->updateConstantBuffer();
}

void Camera::setPosition(Vector3 newPos) {
	this->gPosition = newPos;
	this->dataChanged = true;
}

void Camera::setDirection(Vector3 newDir) {
	this->gDirection = newDir;
	this->dataChanged = true;
}

void Camera::Update() {
	this->shouldUpdateGPUData = false;

	if (this->dataChanged) {
		this->dataChanged = false;

		// Update looking direction vector
		this->gDirection.storeXMVECTOR(
			&dx::XMVector3Transform(
				dx::XMVectorSet(0, 0, 1, 0),
				dx::XMMatrixRotationRollPitchYaw(
					this->rotation.x,
					this->rotation.y,
					0
				)
			)
		);

		this->updateConstantBuffer();
		this->shouldUpdateGPUData = true;
	}
}

void Camera::updateConstantBuffer() {
	//// Update constant buffer on class which is provided to GPU side.
	// Update View matrix.
	dx::XMStoreFloat4x4(
		&this->gCameraVSConstantBuffer.viewMatrix,
		dx::XMMatrixTranspose(
			dx::XMMatrixLookAtLH(
				this->gPosition.loadXMVECTOR(),
				(this->gPosition + this->gDirection).loadXMVECTOR(),
				dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
			)
		)
	);

	// Provide position of the camera to buffer.
	this->gCameraVSConstantBuffer.cameraPosition = this->gPosition.loadXMFLOAT();

	// Update frustum.
	this->frustum = dx::BoundingFrustum(
		dx::XMMatrixTranspose(
			dx::XMLoadFloat4x4(&this->gCameraVSConstantBuffer.projectionMatrix)
		)
	);
	this->frustum.Transform(
		this->frustum,
		dx::XMMatrixInverse(
			NULL,
			dx::XMMatrixTranspose(
				dx::XMLoadFloat4x4(&this->gCameraVSConstantBuffer.viewMatrix)
			)
		)
	);
}

void Camera::Move(Vector3 translation, bool moveFast) {
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
	this->gPosition.x += camTranslation.x;
	this->gPosition.y += camTranslation.y;
	this->gPosition.z += camTranslation.z;

	this->wasMovingFast = moveFast;
	this->dataChanged = true;
}

void Camera::Rotate(float xDelta, float yDelta) {
	xDelta *= this->currentRotationSpeed;
	yDelta *= this->currentRotationSpeed;

	// Update camera rotation
	// xDelta and yDelta comes from mouse's raw movement.
	// Mouse movement on +X axis (on screen space) corresponds to rotation on Y axis on our 3D world.
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

	this->dataChanged = true;
}

void Camera::setPerspectiveProjection(float fieldOfView, float aspectRatio, float nearZ, float farZ) {
	this->projectionType = PERSPECTIVE;

	// Perspective
	dx::XMStoreFloat4x4(
		&this->gCameraVSConstantBuffer.projectionMatrix,
		dx::XMMatrixTranspose(
			dx::XMMatrixPerspectiveFovLH(
			((float)fieldOfView / 360) * (dx::XM_2PI),
				aspectRatio,
				nearZ,
				farZ
			)
		)
	);
	this->shouldUpdateGPUData = true;
}

void Camera::setOrthographicProjection(float viewWidth, float viewHeight, float nearZ, float farZ) {
	this->projectionType = ORTHOGRAPHIC;

	// Orthographic
	dx::XMStoreFloat4x4(
		&this->gCameraVSConstantBuffer.projectionMatrix,
		dx::XMMatrixTranspose(
			dx::XMMatrixOrthographicLH(
				viewWidth, viewHeight,
				nearZ, farZ
			)
		)
	);
	this->shouldUpdateGPUData = true;
}