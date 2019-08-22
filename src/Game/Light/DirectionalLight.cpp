#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Vector3 direction, float intensity) {
	this->type = LIGHT_TYPE::DIRECTIONAL_LIGHT;

	this->gIntensity = intensity;
	this->gDirection = dx::XMFLOAT3(direction.x, direction.y, direction.z);
	this->gPosition = dx::XMFLOAT3(0, 0, 0);

	this->Update(true);
}