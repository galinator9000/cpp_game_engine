#pragma once
#include <vector>

#include "Graphics.h"
#include "Physics.h"
#include "Camera.h"

// Base class for all entities
#include "Entity/BaseEntity.h"
#include "Light/Light.h"

class World{
public:
	World(Graphics* pGfx, Physics* pPhy);

	// General
	void Setup();
	void Update();

	// Entity functions
	void addEntity(BaseEntity* bEntity);

	// Light functions
	void addLight(Light* light, bool activate=true);
	void setLight(Light* light);

	// Camera functions
	void addCamera(Camera* camera, bool setAsMain=false);
	void setCamera(Camera* camera);

	// Texture functions
	void createTexture(Texture* texture);
	void createTextureSampler(TextureSampler* textureSampler);

	// Main camera pointer.
	Camera* activeCamera;

	// List for keeping track of various objects in world.
	std::vector<BaseEntity*> allEntities;
	std::vector<Light*> allLights;
	std::vector<Camera*> allCameras;
	std::vector<Texture*> allTextures;
	std::vector<TextureSampler*> allTextureSamplers;

private:
	Graphics* pGfx;
	Physics* pPhy;
};