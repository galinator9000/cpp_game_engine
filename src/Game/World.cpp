#include "World.h"

World::World(Graphics* pGfx, Physics* pPhy){
	this->pGfx = pGfx;
	this->pPhy = pPhy;
}

void World::Update(){
	// Step and update physics of the world.
	this->pPhy->Update();

	// Clear frame and redraw state of the world.
	this->pGfx->BeginFrame();

	for (unsigned int e = 0; e < allEntities.size(); e++){
		BaseEntity* ent = allEntities[e];

		if(ent == NULL){
			continue;
		}
		if(ent->type == ENTITY_TYPE::PLANE){
			continue;
		}

		this->pGfx->DrawEntity(ent);
	}

	this->pGfx->EndFrame();
}

void World::addEntity(BaseEntity& entity){
	entity.id = (unsigned int) allEntities.size();
	allEntities.push_back(&entity);
	
	this->pPhy->AddEntity(entity);
	this->pGfx->AddEntity(entity);
}