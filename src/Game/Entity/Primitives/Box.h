#pragma once
#include "../BaseEntity.h"
#include "../Structs.h"

using namespace DirectX;

class Box : public BaseEntity {
public:
	Box(Vector3 size, Vector3 position, Vector3 rotation, Vector3 material);
	~Box();
	void Update(bool initial=false);

	// Physics
	PxRigidDynamic* rigidDynamic;

	// Graphics
	void gCreateVerticesAndIndices();
	void updateConstantBuffer();
};