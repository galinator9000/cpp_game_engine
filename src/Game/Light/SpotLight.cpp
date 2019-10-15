#include "SpotLight.h"

SpotLight::SpotLight(Vector3 position, Vector3 direction, float intensity, Color color, float halfSpotAngle) {
	this->type = LIGHT_TYPE::SPOT_LIGHT;

	this->gPosition = position;
	this->gDirection = direction;
	this->gHalfSpotAngle = halfSpotAngle;
	this->gIntensity = intensity;
	this->gDiffuseColor = color;

	this->isActive = true;

	this->updateConstantBuffer();
}

// Set spot light's angle.
void SpotLight::setSpotAngle(float halfSpotAngle) {
	if (this->type != SPOT_LIGHT) {
		return;
	}

	this->gHalfSpotAngle = halfSpotAngle;
	this->dataChanged = true;
}