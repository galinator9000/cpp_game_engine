#include "World.h"

World::World(Graphics* pGfx, Physics* pPhy){
	this->pGfx = pGfx;
	this->pPhy = pPhy;
}

void World::Update(){
	// Step and update physics of the world.
	this->pPhy->Update();

	// Update active camera.
	this->activeCamera->Update();
	this->pGfx->updateCamera(this->activeCamera);

	// Clear frame and redraw state of the world.
	this->pGfx->beginFrame();

	for (unsigned int e = 0; e < allEntities.size(); e++){
		BaseEntity* ent = allEntities[e];

		if(ent == NULL){
			continue;
		}

		ent->Update();

		if(ent->type == ENTITY_TYPE::PLANE){
			continue;
		}

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