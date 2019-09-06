#pragma once
#include "Entity/Entity.h"
#include "Structs.h"

using namespace DirectX;

class Box : public Entity {
public:
	Box(Vector3 size, Vector3 position, Vector4 rotationQ, Color color, Vector3 material, Mesh* mesh = NULL);
	void Update();

	// Physics
	PxRigidDynamic* rigidDynamic;
};