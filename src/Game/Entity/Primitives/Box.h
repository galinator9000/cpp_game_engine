/*#pragma once
#include "Entity/Entity.h"
#include "Structs.h"

using namespace DirectX;

class Box : public BaseEntity {
public:
	Box(Vector3 size, Vector3 position, Vector3 rotation, Color color, Vector3 material);
	~Box();
	void Update();

	// Physics
	PxRigidDynamic* rigidDynamic;

	// Graphics
	void gCreateVerticesAndIndices();
};*/