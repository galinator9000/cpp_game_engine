#pragma once
#define MAX_LIGHT_COUNT 16

#include <vector>

#include "Graphics.h"
#include "Physics.h"
#include "Camera.h"

// Base class for all entities
#include "Entity/Entity.h"
#include "Light/Light.h"

class World{
public:
	World(Graphics* pGfx, Physics* pPhy);

	// General
	void Setup();
	void Reset();
	void Update();

	// Entity functions
	void addEntity(Entity* bEntity);

	// Light functions
	bool addLight(Light* light);

	// Camera functions
	void addCamera(Camera* camera, bool setAsMain=false);
	void setCamera(Camera* camera);

	// Texture functions
	void createTexture(Texture* texture);
	void createTextureSampler(TextureSampler* textureSampler);

	// Main camera pointer.
	Camera* activeCamera;

	// List for keeping track of various objects in world.
	std::vector<Entity*> allEntities;
	std::vector<Light*> allLights;
	std::vector<Camera*> allCameras;
	std::vector<Texture*> allTextures;
	std::vector<TextureSampler*> allTextureSamplers;

	// Buffers for providing multiple lights at the same time.
	LightPSConstantBuffer gAllLightConstantBuffers[MAX_LIGHT_COUNT];
	wrl::ComPtr<ID3D11Buffer> pAllLightConstantBuffers;
	bool shouldUpdateGPUData = false;

private:
	Graphics* pGfx;
	Physics* pPhy;
};