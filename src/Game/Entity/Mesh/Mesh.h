#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <algorithm>

#include "Structs.h"
#include "MeshDeformer/MeshDeformer.h"

class Mesh{
public:
	void Update();

	// Vertex & index
	Vertex* gVertices;
	unsigned int* gIndices;
	unsigned int gVertexCount;
	unsigned int gIndexCount;

	// Mesh deformer (Skeleton)
	MeshDeformer* meshDeformer;
	bool attachMeshDeformer(MeshDeformer* meshDeformer);

	// Graphics
	bool LoadFBX(const char* fileName);

	// Shapes
	void createBoxShape();
};