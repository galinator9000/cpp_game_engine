#pragma once

// Game world
#include "World.h"
#include "Config.h"
#include "InputHandler.h"
#include "Character/Character.h"
#include "Light/DirectionalLight.h"
#include "Light/PointLight.h"
#include "Light/SpotLight.h"

class Game{
public:
	Game(World* pWorld, InputHandler* pInputHandler);
	void Setup();
	void Update();
	void Reset();

	DirectionalLight* directionalSunLight;
	PointLight* pointLight;
	SpotLight* wMainCameraSpotLight;

	Character* mainCharacter = NULL;

	// Base components for a scene.
	World* pWorld;
	InputHandler* pInputHandler;
	Timer timer;

	Camera wMainCamera = Camera(
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3::ZAxis(),
		WIDTH, HEIGHT,
		PROJECTION_TYPE::PERSPECTIVE
	);
	Camera* pwSecondaryCamera;
};