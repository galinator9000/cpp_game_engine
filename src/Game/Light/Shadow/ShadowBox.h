#pragma once
#include "Config.h"
#include "ShadowMap.h"

#include <stdio.h>

class ShadowBox{
public:
	ShadowBox(Vector3 position, Vector3 direction, LIGHT_TYPE lightType);
	void Update(Vector3 position, Vector3 direction);

	LIGHT_TYPE lightType;
	std::vector<ShadowMap*> gShadowMaps;
	bool isActive = false;
};