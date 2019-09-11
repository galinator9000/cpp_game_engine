#pragma once

// Game world
#include "World.h"
#include "Config.h"
#include "Controller.h"
#include "Entity/Character/Character.h"
#include "Light/DirectionalLight.h"
#include "Light/PointLight.h"

class Game{
public:
	Game(World* pWorld, Controller* pMainController);
	void Setup();
	void Update();

	Camera wMainCamera = Camera(
		Vector3(0.0f, 0.0f, 0.0f),
		FOV,
		WIDTH / HEIGHT
	);

	Character* mainCharacter;

	// Base components for a scene.
	World* pWorld;
	Controller* pMainController;
	Timer timer;
};