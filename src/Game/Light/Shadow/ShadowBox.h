#pragma once
#include "Config.h"
#include "Camera.h"
#include "../Graphics/RenderTarget.h"

#include <stdio.h>

class ShadowBox{
public:
	ShadowBox(Vector3 position, Vector3 direction, LIGHT_TYPE lightType);
	void Update(Vector3 position, Vector3 direction);

	LIGHT_TYPE lightType;
	std::vector<std::pair<Camera*, RenderTarget*>> gShadowMaps;
};