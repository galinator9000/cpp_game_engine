#include "PointLight.h"

PointLight::PointLight(Vector3 position, float intensity) {
	this->gPosition = dx::XMFLOAT3(position.x, position.y, position.z);
	this->gIntensity = intensity;

	this->Update(true);
}

// Set light's position.
void PointLight::setPosition(Vector3 newPosition) {
	this->gPosition = dx::XMFLOAT3(newPosition.x, newPosition.y, newPosition.z);
	this->Update();
}

// Set light's intensity.
void PointLight::setIntensity(float intensity) {
	this->gIntensity = intensity;
	this->Update();
}

void PointLight::Update(bool initial) {
	this->updateConstantBuffer();
}

void PointLight::updateConstantBuffer() {
	this->gLightConstBuffer.position = this->gPosition;
	this->gLightConstBuffer.intensity = this->gIntensity;
	this->shouldUpdateData = true;
}