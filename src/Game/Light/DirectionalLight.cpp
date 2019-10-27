#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Vector3 direction, float intensity, Color color, bool shadowCasting) {
	this->type = LIGHT_TYPE::DIRECTIONAL_LIGHT;

	this->gPosition = Vector3(0, 0, 0);
	this->gDirection = direction;
	this->gColor = color;
	this->gIntensity = intensity;
	this->shadowCasting = shadowCasting;

	this->isActive = true;
	this->updateConstantBuffer();
}