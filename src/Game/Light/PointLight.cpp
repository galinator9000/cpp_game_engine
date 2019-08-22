#include "PointLight.h"

PointLight::PointLight(Vector3 position, float intensity){
	this->type = LIGHT_TYPE::POINT_LIGHT;

	this->gIntensity = intensity;
	this->gDirection = dx::XMFLOAT3(0,0,0);
	this->gPosition = dx::XMFLOAT3(position.x, position.y, position.z);

	this->Update(true);
}