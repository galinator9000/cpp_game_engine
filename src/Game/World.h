#pragma once
#include "../Graphics.h"
#include "../Physics.h"

#include "BaseEntity.h"
#include "Cube.h"

class World{
public:
	World(Graphics* pGfx, Physics* pPhy);

	void Update();
	void addEntity(BaseEntity& pBaseEntity);

private:
	Graphics* pGfx;
	Physics* pPhy;
};

