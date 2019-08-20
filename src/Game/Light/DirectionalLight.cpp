#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Vector3 direction, float intensity) {
	this->gDirection = dx::XMFLOAT3(direction.x, direction.y, direction.z);
	this->gIntensity = intensity;

	this->gLightConstBuffer.direction = this->gDirection;
	this->gLightConstBuffer.intensity = this->gIntensity;

	this->type = LIGHT_TYPE::DIRECTIONAL;
}