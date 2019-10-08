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
	void switchCamera();
	// Active camera pointer.
	Camera* activeCamera = NULL;

	// Texture functions
	void createTexture(Texture* texture);
	void createTextureSampler(TextureSampler* textureSampler);

	// Physics
	bool createFixedJoint(
		Entity* entity1, Entity* entity2,
		Vector3 transform1 = Vector3(0, 0, 0), Vector3 transform2 = Vector3(0, 0, 0)
	);
	bool createDistanceJoint(
		Entity* entity1, Entity* entity2,
		Vector3 transform1 = Vector3(0, 0, 0), Vector3 transform2 = Vector3(0, 0, 0)
	);
	bool createSphericalJoint(
		Entity* entity1, Entity* entity2,
		Vector3 transform1 = Vector3(0, 0, 0), Vector3 transform2 = Vector3(0, 0, 0)
	);
	bool createRevoluteJoint(
		Entity* entity1, Entity* entity2,
		Vector3 transform1 = Vector3(0, 0, 0), Vector3 transform2 = Vector3(0, 0, 0)
	);

	// List for keeping track of various objects in world.
	std::vector<Entity*> allEntities;
	std::vector<Light*> allLights;
	std::vector<Camera*> allCameras;
	std::vector<Texture*> allTextures;
	std::vector<TextureSampler*> allTextureSamplers;

	// Buffers for providing multiple lights at the same time.
	LightPSConstantBuffer gAllLightConstantBuffers[MAX_LIGHT_COUNT];
	wrl::ComPtr<ID3D11Buffer> pAllLightConstantBuffers;

private:
	Graphics* pGfx;
	Physics* pPhy;
};