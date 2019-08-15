#include "Camera.h"

Camera::Camera(float posX, float posY, float posZ, unsigned int fov, float aspectRatio){
	this->gFieldOfView = fov;
	this->gAspectRatio = aspectRatio;

	this->eye = dx::XMFLOAT3(posX, posY, posZ);
	this->at = dx::XMFLOAT3(eye.x, eye.y, eye.z + 1.0f);
	this->up = dx::XMFLOAT3(0, 1, 0);

	this->Update(true);
}

void Camera::Update(bool initial){
	this->eye = dx::XMFLOAT3(this->eye.x + 0.002f, this->eye.y - 0.001f, this->eye.z - 0.002f);
	this->at = dx::XMFLOAT3(eye.x, eye.y, eye.z + 1.0f);
	this->hasChanged = true;

	this->shouldUpdateData = true;

	if (!initial && !hasChanged){
		this->shouldUpdateData = false;
		return;
	}

	this->updateConstantBuffer();
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
}