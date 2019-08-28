#include "Light.h"

// Set light's intensity.
void Light::setIntensity(float intensity){
	this->gIntensity = intensity;
	this->Update();
}

// Set light's direction.
void Light::setDirection(Vector3 newDirection) {
	if (this->type == POINT_LIGHT) {
		return;
	}

	this->gDirection = dx::XMFLOAT3(newDirection.x, newDirection.y, newDirection.z);
	this->Update();
}

// Set light's position.
void Light::setPosition(Vector3 newPosition) {
	if (this->type == DIRECTIONAL_LIGHT) {
		return;
	}

	this->gPosition = dx::XMFLOAT3(newPosition.x, newPosition.y, newPosition.z);
	this->Update();
}

void Light::Update(bool initial) {
	this->updateConstantBuffer();
}

void Light::updateConstantBuffer() {
	this->gLightConstantBuffer.type = this->type;
	this->gLightConstantBuffer.intensity = this->gIntensity;
	this->gLightConstantBuffer.direction = this->gDirection;
	this->gLightConstantBuffer.position = this->gPosition;
	this->shouldUpdateData = true;
}