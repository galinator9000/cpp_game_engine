#pragma once
#include "Config.h"
#include "ShadowMap.h"
#include "../Utils/General.h"

#include <directxcollision.h>
#include <stdio.h>
#include <cmath>

class ShadowBox{
public:
	ShadowBox(Vector3 position, Vector3 direction, Vector2 mapDimensions, LIGHT_TYPE lightType, unsigned int shadowMapCount=1);
	void Update(Vector3 position, Vector3 direction, Camera* activeCamera);

	LIGHT_TYPE lightType;
	unsigned int gShadowMapCount = 1;
	ShadowMap** gShadowMaps;
	bool isActive = false;

	// Distance
	float gShadowDistanceRatio = 0.66f;
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