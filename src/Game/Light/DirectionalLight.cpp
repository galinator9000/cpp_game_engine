#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Vector3 direction, float intensity, Color color) {
	this->type = LIGHT_TYPE::DIRECTIONAL_LIGHT;

	this->gDirection = dx::XMFLOAT3(direction.x, direction.y, direction.z);
	this->gIntensity = intensity;
	this->gDiffuseColor = dx::XMFLOAT4(color.r, color.g, color.b, color.a);

	this->gPosition = dx::XMFLOAT3(0, 0, 0);
	this->updateConstantBuffer();
}