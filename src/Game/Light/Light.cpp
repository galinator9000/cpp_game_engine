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
	this->gLightConstBuffer.type = this->type;
	this->gLightConstBuffer.intensity = this->gIntensity;
	this->gLightConstBuffer.direction = this->gDirection;
	this->gLightConstBuffer.position = this->gPosition;
	this->shouldUpdateData = true;
}