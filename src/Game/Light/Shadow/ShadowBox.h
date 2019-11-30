#pragma once
#include "Config.h"
#include "ShadowMap.h"
#include <directxcollision.h>

#include <stdio.h>

class ShadowBox{
public:
	ShadowBox(Vector3 position, Vector3 direction, LIGHT_TYPE lightType);
	void Update(Vector3 position, Vector3 direction, Camera* activeCamera);

	LIGHT_TYPE lightType;
	ShadowMap* gShadowMap;
	bool isActive = false;

	float gShadowDistanceRatio = 0.2f;
	void setShadowDistanceRatio(float ratio) {
		if (ratio > 0 && ratio <= 1) {
			this->gShadowDistanceRatio = ratio;
		}
	}
};