#include "Camera.h"

Camera::Camera(float posX, float posY, float posZ, unsigned int fov, float aspectRatio){
	this->gFieldOfView = fov;
	this->gAspectRatio = aspectRatio;

	this->eye = dx::XMFLOAT3(posX, posY, posZ);
	this->at = dx::XMFLOAT3(this->eye.x, this->eye.y, this->eye.z + 1.0f);
	this->up = dx::XMFLOAT3(0, 1, 0);
	this->directionAccumulate = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);

	this->Update(true);
}

void Camera::moveDirection(float x, float y, float z){
	this->directionAccumulate.x += this->currentSpeed * x;
	this->directionAccumulate.y += this->currentSpeed * y;
	this->directionAccumulate.z += this->currentSpeed * z;
	this->hasChanged = true;
}

void Camera::Update(bool initial){
	this->shouldUpdateData = false;

	if (initial){
		this->updateConstantBuffer();
	}

	if (this->hasChanged) {
		// Update camera position and orientation data.
		this->eye.x += this->directionAccumulate.x;
		this->eye.y += this->directionAccumulate.y;
		this->eye.z += this->directionAccumulate.z;
		this->at = dx::XMFLOAT3(this->eye.x, this->eye.y, this->eye.z + 1.0f);

		// Reset.
		this->directionAccumulate = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
		this->hasChanged = false;

		this->updateConstantBuffer();
	}
}

void Camera::updateConstantBuffer() {
	// Update constant buffer that held on class.
	// View matrix
	dx::XMStoreFloat4x4(
		&this->gViewProjection.viewMatrix,
		dx::XMMatrixTranspose(
			dx::XMMatrixLookAtLH(
				dx::XMLoadFloat3(&this->eye),
				dx::XMLoadFloat3(&this->at),
				dx::XMLoadFloat3(&this->up)
			)
		)
	);

	// Projection
	dx::XMStoreFloat4x4(
		&this->gViewProjection.projectionMatrix,
		dx::XMMatrixTranspose(
			dx::XMMatrixPerspectiveFovLH(
			((float)this->gFieldOfView / 360) * (2.0f * dx::XM_PI),
				this->gAspectRatio,
				0.5f,
				100.0f
			)
		)
	);
	this->shouldUpdateData = true;
}