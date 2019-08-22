#include "PointLight.h"

PointLight::PointLight(Vector3 position, float intensity) {
	this->gPosition = dx::XMFLOAT3(position.x, position.y, position.z);
	this->gIntensity = intensity;

	this->gLightConstBuffer.position = this->gPosition;
	this->gLightConstBuffer.intensity = this->gIntensity;
}