#pragma once
#include "../Graphics.h"
#include "../Physics.h"

// Base class for all entities
#include "BaseEntity.h"

// Base class for basic shapes
#include "Box.h"
#include "Plane.h"

class World{
public:
	World(Graphics* pGfx, Physics* pPhy);

	void Update();
	void addEntity(BaseEntity&);

private:
	Graphics* pGfx;
	Physics* pPhy;
};

