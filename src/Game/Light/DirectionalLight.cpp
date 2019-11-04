#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Vector3 direction, float intensity, Color color, bool isCastingShadow) {
	this->type = LIGHT_TYPE::DIRECTIONAL_LIGHT;

	this->gPosition = Vector3(0, 0, 0);
	this->gDirection = direction;
	this->gColor = color;
	this->gIntensity = intensity;

	this->isCastingShadow = isCastingShadow;
	if (this->isCastingShadow) {
		this->gShadowBox = new ShadowBox(this->gPosition, this->gDirection, this->type);
	}

	this->isActive = true;
	this->updateConstantBuffer();
}