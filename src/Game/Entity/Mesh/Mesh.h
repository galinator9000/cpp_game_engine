#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <algorithm>

#include "Structs.h"
#include "MeshDeformer.h"

class Mesh{
public:
	// Vertex & index
	Vertex* gVertices;
	unsigned int* gIndices;
	unsigned int gVertexCount;
	unsigned int gIndexCount;

	// Deformer (Skeleton)
	MeshDeformer* meshDeformer;
	bool attachMeshDeformer(MeshDeformer* meshDeformer);

	// Graphics
	bool LoadFBX(const char* fileName);

	// Shapes
	void createBoxShape();
};