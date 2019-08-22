#pragma once

// Game world
#include "World.h"
#include "Config.h"
#include "Light/DirectionalLight.h"
#include "Light/PointLight.h"

class Game{
public:
	Game(World* pWorld);
	void Setup();
	void Update();

	Timer timer;
	World* pWorld;
};