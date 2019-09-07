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

	Camera wMainCamera = Camera(
		Vector3(0.0f, 0.0f, 0.0f),
		FOV,
		WIDTH / HEIGHT
	);

	Entity* animatedEntity;

	Entity* parentBoxEntity;
	Entity* childBoxEntity1;
	Entity* childBoxEntity2;
	Entity* childBoxEntity3;
	Entity* childBoxEntity4;

	Timer timer;
	World* pWorld;
};