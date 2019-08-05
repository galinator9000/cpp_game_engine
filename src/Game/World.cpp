#include "World.h"

World::World(Graphics* pGfx, Physics* pPhy){
	this->pGfx = pGfx;
	this->pPhy = pPhy;
}

void World::Update(){
	// Clear frame and redraw state of the world.
	this->pGfx->BeginFrame();
	this->pGfx->Draw();
	this->pGfx->EndFrame();

	// Step and update physics of the world.
	this->pPhy->Update();
}

void World::addEntity(BaseEntity& pBaseEntity){
	this->pPhy->AddEntity(pBaseEntity);
	// Add entity to graphics.
}