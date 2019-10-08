#include "PointLight.h"

PointLight::PointLight(Vector3 position, float intensity, Color color){
	this->type = LIGHT_TYPE::POINT_LIGHT;

	this->gPosition = dx::XMFLOAT3(position.x, position.y, position.z);
	this->gIntensity = intensity;
	this->gDiffuseColor = dx::XMFLOAT4(color.r, color.g, color.b, color.a);

	this->gDirection = dx::XMFLOAT3(0, 0, 0);

	this->updateConstantBuffer();
}