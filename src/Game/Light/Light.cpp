#include "Light.h"

// Set light's intensity.
void Light::setIntensity(float intensity) {
	this->gIntensity = intensity;
	this->dataChanged = true;
}

// Set light's color.
void Light::setColor(Color color) {
	this->gColor = color;
	this->dataChanged = true;
}

// Set light's direction.
void Light::setDirection(Vector3 newDirection) {
	if (this->type == POINT_LIGHT) {
		return;
	}

	this->gDirection = newDirection;
	this->dataChanged = true;
}

// Set light's position.
void Light::setPosition(Vector3 newPosition) {
	if (this->type == DIRECTIONAL_LIGHT) {
		return;
	}

	this->gPosition = newPosition;
	this->dataChanged = true;
}

void Light::Reset() {
	this->dataChanged = false;
}

void Light::Update() {
	if (this->dataChanged) {
		this->updateConstantBuffer();
		this->dataChanged = false;
	}
}

void Light::updateConstantBuffer() {
	this->shouldUpdateGPUData = true;
}