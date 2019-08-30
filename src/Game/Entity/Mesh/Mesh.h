#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <algorithm>

#include "Structs.h"

class Mesh{
public:
	// Vertex & index
	Vertex* gVertices;
	unsigned int* gIndices;
	unsigned int gVertexCount;
	unsigned int gIndexCount;

	// Shapes
	void createBoxShape();

	// Graphics
	bool LoadFBX(const char* fileName);
};