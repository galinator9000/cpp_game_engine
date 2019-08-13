#include "World.h"

World::World(Graphics* pGfx, Physics* pPhy){
	this->pGfx = pGfx;
	this->pPhy = pPhy;
}

void World::Update(){
	// Step and update physics of the world.
	this->pPhy->Update();

	// Clear frame and redraw state of the world.
	this->pGfx->beginFrame();

	for (unsigned int e = 0; e < allEntities.size(); e++){
		BaseEntity* ent = allEntities[e];

		if(ent->type == ENTITY_TYPE::PLANE || ent == NULL){
			continue;
		}

		this->pGfx->drawEntity(ent);
	}

	this->pGfx->endFrame();
}

void World::addEntity(BaseEntity* bEntity){
	bEntity->id = (unsigned int) allEntities.size();
	allEntities.push_back(bEntity);
	
	this->pPhy->addEntity(bEntity);
	this->pGfx->addEntity(bEntity);
}