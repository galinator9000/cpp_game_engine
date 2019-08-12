#pragma once
#include <vector>

#include "../Graphics.h"
#include "../Physics.h"

// Base class for all entities
#include "BaseEntity.h"

class World{
public:
	World(Graphics* pGfx, Physics* pPhy);

	void Update();
	void addEntity(BaseEntity&);

private:
	Graphics* pGfx;
	Physics* pPhy;

	std::vector<BaseEntity*> allEntities;
};