#include "PointLight.h"

PointLight::PointLight(Vector3 position, float intensity, Color color){
	this->type = LIGHT_TYPE::POINT_LIGHT;

	this->gPosition = position;
	this->gDirection = Vector3(0, 0, 0);
	this->gColor = color;
	this->gIntensity = intensity;

	this->isActive = true;
	this->updateConstantBuffer();
}