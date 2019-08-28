#include "World.h"

World::World(Graphics* pGfx, Physics* pPhy){
	this->pGfx = pGfx;
	this->pPhy = pPhy;
	this->Setup();
}

void World::Setup() {

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
		this->pGfx->updateLight(light);
	}

	// Update active camera.
	this->activeCamera->Update();
	this->pGfx->updateCamera(this->activeCamera);

	//// Draw section.
	// Clear frame and redraw state of the world.
	this->pGfx->beginFrame();

	// Update and draw all entities.
	for (unsigned int e = 0; e < allEntities.size(); e++){
		BaseEntity* ent = allEntities.at(e);

		if(ent == NULL){
			continue;
		}

		ent->Update();
		this->pGfx->updateEntity(ent);
		this->pGfx->drawEntity(ent);
		ent->Reset();
	}

	this->pGfx->endFrame();
}

// Entity
void World::addEntity(BaseEntity* bEntity){
	bEntity->id = (unsigned int) allEntities.size();
	allEntities.push_back(bEntity);
	
	this->pPhy->addEntity(bEntity);
	this->pGfx->addEntity(bEntity);
}

// Light
void World::addLight(Light* light, bool activate) {
	light->id = (unsigned int) allLights.size();
	allLights.push_back(light);
	this->pGfx->addLight(light, activate);
}

void World::setLight(Light* light) {
	this->pGfx->activateLight(light);
}

// Camera
void World::addCamera(Camera* camera, bool setAsMain){
	camera->id = (unsigned int) allCameras.size();
	allCameras.push_back(camera);

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
	allTextures.push_back(texture);
	this->pGfx->createTextureDDS(texture);
}

void World::createTextureSampler(TextureSampler* textureSampler){
	textureSampler->id = allTextureSamplers.size();
	allTextureSamplers.push_back(textureSampler);
	this->pGfx->createTextureSampler(textureSampler);
}