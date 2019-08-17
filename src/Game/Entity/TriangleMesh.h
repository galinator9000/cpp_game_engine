#pragma once
#include "BaseEntity.h"
#include "Structs.h"

using namespace DirectX;

class TriangleMesh : public BaseEntity {
public:
	TriangleMesh(PxVec3 size, PxVec3 position, PxVec3 rotation, PxVec3 material);
	~TriangleMesh();
	void Update(bool initial = false);

	// Graphics
	void updateConstantBuffer();

	bool LoadVerticesAndIndicesOBJ(const char* fileName);
};