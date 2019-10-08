#include "Light.h"

// Set light's intensity.
void Light::setIntensity(float intensity){
	this->gIntensity = intensity;
	this->hasChanged = true;
}

// Set light's color.
void Light::setColor(Color color) {
	this->gDiffuseColor = dx::XMFLOAT3(color.r, color.g, color.b);
	this->hasChanged = true;
}

// Set light's direction.
void Light::setDirection(Vector3 newDirection) {
	if (this->type == POINT_LIGHT) {
		return;
	}

	this->gDirection = dx::XMFLOAT3(newDirection.x, newDirection.y, newDirection.z);
	this->hasChanged = true;
}

// Set light's position.
void Light::setPosition(Vector3 newPosition) {
	if (this->type == DIRECTIONAL_LIGHT) {
		return;
	}

	this->gPosition = dx::XMFLOAT3(newPosition.x, newPosition.y, newPosition.z);
	this->hasChanged = true;
}

void Light::Reset() {
	this->hasChanged = false;
}

void Light::Update() {
	if (this->hasChanged) {
		this->updateConstantBuffer();
		this->hasChanged = false;
	}
}

void Light::updateConstantBuffer() {
	this->shouldUpdateGPUData = true;
}