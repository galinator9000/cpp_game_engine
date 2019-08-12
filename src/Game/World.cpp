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

		if(ent == NULL){
			continue;
		}
		if(ent->type == ENTITY_TYPE::PLANE){
			continue;
		}

		this->pGfx->drawEntity(*ent);
	}

	this->pGfx->endFrame();
}

void World::addEntity(BaseEntity& entity){
	entity.id = (unsigned int) allEntities.size();
	allEntities.push_back(&entity);
	
	this->pPhy->addEntity(entity);
	this->pGfx->addEntity(entity);
}