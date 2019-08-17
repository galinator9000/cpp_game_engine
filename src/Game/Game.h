#pragma once

// Game world
#include "World.h"

#include "Config.h"

class Game{
public:
	Game(World* pWorld);
	void Setup();
	void Update();

	World* pWorld;
};