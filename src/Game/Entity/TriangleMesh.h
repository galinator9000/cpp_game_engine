#pragma once
#include "BaseEntity.h"
#include "Structs.h"

using namespace DirectX;

class TriangleMesh : public BaseEntity {
public:
	TriangleMesh(Vector3 size, Vector3 position, Vector3 rotation, Vector3 material);
	~TriangleMesh();
	void Update(bool initial = false);

	// Graphics
	void updateConstantBuffer();

	bool LoadVerticesAndIndicesFBX(const char* fileName);
};