#include "World.h"

World::World(Graphics* pGfx, Physics* pPhy){
	this->pGfx = pGfx;
	this->pPhy = pPhy;
}

void World::Update(){
	//// Update section.
	// Step and update physics of the world.
	this->pPhy->Update();

	// Update all light objects.
	for (unsigned int l = 0; l < allLights.size(); l++) {
		PointLight* light = allLights[l];

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
		BaseEntity* ent = allEntities[e];

		if(ent == NULL){
			continue;
		}

		ent->Update();

		this->pGfx->drawEntity(ent);
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
void World::addLight(PointLight* light, bool activate) {
	light->id = (unsigned int) allLights.size();
	allLights.push_back(light);
	this->pGfx->addLight(light, activate);
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
	this->pGfx->setCamera(this->activeCamera);
}