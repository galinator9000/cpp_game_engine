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

	DirectionalLight* directionalSunLight;
	PointLight* pointLight;
	SpotLight* wMainCameraSpotLight;

	Character* mainCharacter = NULL;

	// Base components for a scene.
	World* pWorld;
	Controller* pMainController;
	Timer timer;

	Camera wMainCamera = Camera(
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3::ZAxis(),
		WIDTH, HEIGHT,
		PROJECTION_TYPE::PERSPECTIVE
	);
	Camera* pwSecondaryCamera;
};