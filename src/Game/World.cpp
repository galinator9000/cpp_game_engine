#include "World.h"

World::World(Graphics* pGfx, Physics* pPhy){
	this->pGfx = pGfx;
	this->pPhy = pPhy;
	this->Setup();
}

void World::Setup() {
	// Create buffer that will hold multiple lights.
	// Set undefined lights' intensity to -1.
	for (unsigned int light = 0; light<MAX_LIGHT_COUNT; light++) {
		gAllLightConstantBuffers[light].intensity = -1.0f;
	}

	this->pGfx->createLightsBuffer(
		&this->gAllLightConstantBuffers[0],
		MAX_LIGHT_COUNT,
		&this->pAllLightConstantBuffers
	);
	this->pGfx->bindLightsBuffer(this->pAllLightConstantBuffers.Get());
}

void World::Reset() {
	this->shouldUpdateGPUData = false;
}

void World::Update(){
	//// Update section.
	// Step and update physics of the world.
	this->pPhy->Update();

	// Update all light objects.
	for (unsigned int l = 0; l < allLights.size(); l++) {
		Light* light = allLights.at(l);

		if (light == NULL) {
			continue;
		}

		light->Update();

		if (light->shouldUpdateGPUData) {
			// Update data for each light.
			this->gAllLightConstantBuffers[light->id].intensity = light->gIntensity;
			this->gAllLightConstantBuffers[light->id].direction = light->gDirection;
			this->gAllLightConstantBuffers[light->id].position = light->gPosition;
			this->gAllLightConstantBuffers[light->id].type = light->type;

			light->shouldUpdateGPUData = false;
			this->shouldUpdateGPUData = true;
		}
	}

	// Update lights buffer on GPU side.
	if (this->shouldUpdateGPUData) {
		this->pGfx->updateLightsBuffer(
			&this->gAllLightConstantBuffers[0],
			MAX_LIGHT_COUNT,
			this->pAllLightConstantBuffers.Get()
		);
		this->shouldUpdateGPUData = false;
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
	entity->id = (unsigned int)this->allEntities.size();
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

	if(setAsMain){
		this->setCamera(camera);
	}

	this->pGfx->addCamera(camera, setAsMain);
}

void World::setCamera(Camera* camera) {
	this->activeCamera = camera;
	this->pGfx->activateCamera(this->activeCamera);
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