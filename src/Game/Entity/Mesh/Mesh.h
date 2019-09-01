#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <algorithm>
#include <map>

#include "Structs.h"
#include "MeshDeformer/MeshDeformer.h"

namespace wrl = Microsoft::WRL;

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

	// Constant buffers on GPU side
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	bool hasGPUBuffers = false;

	// Shapes
	void createBoxShape();
};