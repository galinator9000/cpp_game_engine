#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Vector3 direction, float intensity, Color color) {
	this->type = LIGHT_TYPE::DIRECTIONAL_LIGHT;

	this->gDirection = direction;
	this->gIntensity = intensity;
	this->gDiffuseColor = color;

	this->gPosition = Vector3(0, 0, 0);
	this->updateConstantBuffer();
}