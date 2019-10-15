#include "World.h"

World::World(Graphics* pGfx, Physics* pPhy, Audio* pAud){
	this->pGfx = pGfx;
	this->pPhy = pPhy;
	this->pAud = pAud;

	this->Setup();
}

void World::Setup() {
	// Create buffer that will hold multiple lights.
	// Set undefined lights' intensity to -1.
	for (unsigned int light = 0; light<MAX_LIGHT_COUNT; light++) {
		gAllLightConstantBuffers[light].intensity = -1.0f;

		gAllLightConstantBuffers[light].color = dx::XMFLOAT4(0, 0, 0, 0);
		gAllLightConstantBuffers[light].direction = dx::XMFLOAT3(0, 0, 0);
		gAllLightConstantBuffers[light].position = dx::XMFLOAT3(0, 0, 0);
		gAllLightConstantBuffers[light].halfSpotAngle = 0;
	}

	this->pGfx->createLightsBuffer(
		&this->gAllLightConstantBuffers[0],
		MAX_LIGHT_COUNT,
		&this->pAllLightConstantBuffers
	);
	this->pGfx->bindLightsBuffer(this->pAllLightConstantBuffers.Get());
}

void World::Reset() {

}

void World::Update(){
	//// Update section.
	// Step and update physics of the world.
	this->pPhy->Update();

	// Update vector relations.
	for (unsigned int vr = 0; vr < allVectorRelations.size(); vr++) {
		allVectorRelations.at(vr)->Update();
	}

	// Update all light objects.
	bool shouldUpdateLightsGPUData = false;
	for (unsigned int l = 0; l < allLights.size(); l++) {
		Light* light = allLights.at(l);

		if (light == NULL) {
			continue;
		}

		light->Update();

		if (light->shouldUpdateGPUData) {
			// Update data for each light.
			this->gAllLightConstantBuffers[light->id].type = light->type;
			this->gAllLightConstantBuffers[light->id].isActive = light->isActive;
			this->gAllLightConstantBuffers[light->id].intensity = light->gIntensity;
			this->gAllLightConstantBuffers[light->id].halfSpotAngle = light->gHalfSpotAngle;
			this->gAllLightConstantBuffers[light->id].color = light->gDiffuseColor.loadXMFLOAT();
			this->gAllLightConstantBuffers[light->id].position = light->gPosition.loadXMFLOAT();
			this->gAllLightConstantBuffers[light->id].direction = light->gDirection.loadXMFLOAT();

			light->shouldUpdateGPUData = false;
			shouldUpdateLightsGPUData = true;
		}
	}

	// Update lights buffer on GPU side.
	if (shouldUpdateLightsGPUData) {
		this->pGfx->updateLightsBuffer(
			&this->gAllLightConstantBuffers[0],
			MAX_LIGHT_COUNT,
			this->pAllLightConstantBuffers.Get()
		);
		shouldUpdateLightsGPUData = false;
	}

	// Update active camera.
	this->activeCamera->Update();
	this->pGfx->updateCamera(this->activeCamera);

	//// Draw section.
	// Clear frame and redraw state of the world.
	this->pGfx->beginFrame();

	// Update and draw all entities.
	for (unsigned int e = 0; e < this->allEntities.size(); e++){
		Entity* ent = this->allEntities.at(e);

		if(ent == NULL){
			continue;
		}

		this->pPhy->updateEntity(ent);
		ent->Update();

		this->pGfx->updateEntity(ent);
		this->pGfx->drawEntity(ent);

		ent->Reset();
	}

	this->pGfx->endFrame();

	//// Reset section
	for (unsigned int l = 0; l < allLights.size(); l++) {
		Light* light = allLights.at(l);

		if (light == NULL) {
			continue;
		}

		light->Reset();
	}
}

// Entity
void World::addEntity(Entity* entity){
	entity->id = (unsigned int) this->allEntities.size();
	this->allEntities.push_back(entity);
	
	this->pPhy->addEntity(entity);
	this->pGfx->addEntity(entity);
}

// Light
bool World::addLight(Light* light) {
	if (this->allLights.size() >= MAX_LIGHT_COUNT) {
		return false;
	}

	light->id = (unsigned int)this->allLights.size();
	this->allLights.push_back(light);

	return true;
}

// Camera
void World::addCamera(Camera* camera, bool setAsMain){
	camera->id = (unsigned int)this->allCameras.size();
	this->allCameras.push_back(camera);

	setAsMain = (setAsMain || this->activeCamera == NULL);

	if (setAsMain) {
		this->setCamera(camera);
	}
	this->pGfx->addCamera(camera, setAsMain);
}

void World::setCamera(Camera* camera) {
	this->activeCamera = camera;
	this->pGfx->activateCamera(this->activeCamera);
}

void World::switchCamera() {
	// If there's only one camera, simply return.
	if (this->allCameras.size() <= 1) {
		return;
	}

	unsigned int nextCamID = this->activeCamera->id;
	while(nextCamID <= (unsigned int) this->allCameras.size()-1){
		nextCamID++;
		if (nextCamID >= this->allCameras.size()) {
			nextCamID = 0;
		}

		if (this->allCameras.at(nextCamID) != NULL) {
			break;
		}
	}

	this->setCamera(this->allCameras.at(nextCamID));
}

// Texture
void World::createTexture(Texture* texture) {
	this->allTextures.push_back(texture);
	this->pGfx->createTextureDDS(texture);
}

void World::createTextureSampler(TextureSampler* textureSampler){
	textureSampler->id = (unsigned int) this->allTextureSamplers.size();
	this->allTextureSamplers.push_back(textureSampler);
	this->pGfx->createTextureSampler(textureSampler);
}

// Vector Relation
void World::addVectorRelation(VectorRelation* vectorRelation) {
	vectorRelation->id = (unsigned int) this->allVectorRelations.size();
	this->allVectorRelations.push_back(vectorRelation);
}

// Physics
bool World::createFixedJoint(
	Entity* entity1, Entity* entity2,
	Vector3 transform1, Vector3 transform2
){
	return this->pPhy->createFixedJoint(
		entity1->pCollisionActor, entity2->pCollisionActor,
		transform1.toPxTransform(),
		transform2.toPxTransform()
	);
}

bool World::createDistanceJoint(
	Entity* entity1, Entity* entity2,
	Vector3 transform1, Vector3 transform2
){
	return this->pPhy->createDistanceJoint(
		entity1->pCollisionActor, entity2->pCollisionActor,
		transform1.toPxTransform(),
		transform2.toPxTransform()
	);
}

bool World::createSphericalJoint(
	Entity* entity1, Entity* entity2,
	Vector3 transform1, Vector3 transform2
){
	return this->pPhy->createSphericalJoint(
		entity1->pCollisionActor, entity2->pCollisionActor,
		transform1.toPxTransform(),
		transform2.toPxTransform()
	);
}

bool World::createRevoluteJoint(
	Entity* entity1, Entity* entity2,
	Vector3 transform1, Vector3 transform2
) {
	return this->pPhy->createRevoluteJoint(
		entity1->pCollisionActor, entity2->pCollisionActor,
		transform1.toPxTransform(),
		transform2.toPxTransform()
	);
}