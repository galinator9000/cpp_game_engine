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

	// Distance
	float gShadowDistanceRatio = 0.33f;
	void setShadowDistanceRatio(float ratio) {
		if (ratio > 0 && ratio <= 1) {
			this->gShadowDistanceRatio = ratio;
		}
	}
	float getShadowDistance() {
		return this->gShadowDistance;
	}

protected:
	float gShadowDistance = 100.0f;
};