#pragma once

// Game world
#include "World.h"
#include "Config.h"
#include "Controller.h"
#include "Entity/Character/Character.h"
#include "Light/DirectionalLight.h"
#include "Light/PointLight.h"
#include "Light/SpotLight.h"

class Game{
public:
	Game(World* pWorld, Controller* pMainController);
	void Setup();
	void Update();

	Character* mainCharacter = NULL;

	// Base components for a scene.
	World* pWorld;
	Controller* pMainController;
	Timer timer;

	Camera wMainCamera = Camera(
		Vector3(0.0f, 0.0f, 0.0f),
		FOV,
		WIDTH / HEIGHT
	);
	Camera* pwSecondaryCamera;
	SpotLight* wMainCameraSpotLight;
};