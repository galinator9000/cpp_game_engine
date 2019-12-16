#pragma once

#include <vector>

#include "Graphics/Graphics.h"
#include "Physics/Physics.h"
#include "Audio/Audio.h"

// Base class for all entities
#include "Camera.h"
#include "VectorRelation.h"
#include "Entity/Entity.h"
#include "Light/Light.h"

class World{
public:
	World(Graphics* pGfx, Physics* pPhy, Audio* pAud);

	// General
	void Setup();
	void Reset();
	void Update();
	void Render();

	// Entity functions
	bool addEntity(Entity* bEntity);

	// Light functions
	bool addLight(Light* light);

	// Camera functions
	bool addCamera(Camera* camera, bool setAsMain=false);
	bool setCamera(Camera* camera);
	void switchCamera();
	// Active camera pointer.
	Camera* activeCamera = NULL;

	// Texture functions
	void createTexture(Texture* texture);

	// Vector relation functions
	void addVectorRelation(VectorRelation* vectorRelation);

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
	std::vector<VectorRelation*> allVectorRelations;

	// Buffers for providing multiple lights at the same time.
	LightPSStruct gAllLightConstantBuffers[MAX_LIGHT_COUNT];
	wrl::ComPtr<ID3D11Buffer> pAllLightConstantBuffers;

	// Buffer for providing shadow casters.
	ShadowBoxSStruct gAllShadowBoxConstantBuffers[MAX_SHADOWBOX_COUNT];
	wrl::ComPtr<ID3D11Buffer> pAllShadowBoxConstantBuffers;

	Light* gShadowCasters[MAX_SHADOWBOX_COUNT];
	std::map<float, Light*> gShadowCastersDistanceLPMap;

private:
	Graphics* pGfx;
	Physics* pPhy;
	Audio* pAud;
};