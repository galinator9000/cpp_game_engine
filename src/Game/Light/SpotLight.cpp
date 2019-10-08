#include "SpotLight.h"

SpotLight::SpotLight(Vector3 position, Vector3 direction, float intensity, Color color, float halfSpotAngle) {
	this->type = LIGHT_TYPE::SPOT_LIGHT;

	this->gPosition = dx::XMFLOAT3(position.x, position.y, position.z);
	this->gDirection = dx::XMFLOAT3(direction.x, direction.y, direction.z);
	this->gHalfSpotAngle = halfSpotAngle;
	this->gIntensity = intensity;
	this->gDiffuseColor = dx::XMFLOAT4(color.r, color.g, color.b, color.a);

	this->updateConstantBuffer();
}

// Set spot light's angle.
void SpotLight::setSpotAngle(float halfSpotAngle) {
	if (this->type != SPOT_LIGHT) {
		return;
	}

	this->gHalfSpotAngle = halfSpotAngle;
	this->hasChanged = true;
}