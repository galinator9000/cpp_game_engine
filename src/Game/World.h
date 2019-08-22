#pragma once
#include <vector>

#include "Graphics.h"
#include "Physics.h"
#include "Camera.h"

// Base class for all entities
#include "Entity/BaseEntity.h"
#include "Light/DirectionalLight.h"
#include "Light/PointLight.h"

class World{
public:
	World(Graphics* pGfx, Physics* pPhy);

	// General
	void Update();

	// Entity functions
	void addEntity(BaseEntity* bEntity);

	// Light functions
	void addLight(PointLight* light, bool activate=true);

	// Camera functions
	void addCamera(Camera* camera, bool setAsMain=false);
	void setCamera(Camera* camera);

	// Main camera pointer.
	Camera* activeCamera;

	// List for keeping track of entities.
	std::vector<BaseEntity*> allEntities;

	// List for keeping track of lights.
	std::vector<PointLight*> allLights;

	// List for keeping track of cameras.
	std::vector<Camera*> allCameras;

private:
	Graphics* pGfx;
	Physics* pPhy;
};