#pragma once
#include "Entity/Entity.h"
#include "Structs.h"

class Plane : public Entity{
public:
	// Physics
	Plane(Vector3 normals, Vector3 material);
};