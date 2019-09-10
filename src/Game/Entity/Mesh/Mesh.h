#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <algorithm>
#include <map>

#include "Structs.h"
#include "MeshDeformer/Animation/Animation.h"

namespace wrl = Microsoft::WRL;

class Mesh{
public:
	// Vertex & index
	Vertex* gVertices;
	unsigned int* gIndices;
	unsigned int gVertexCount;
	unsigned int gIndexCount;

	// Skeleton class which holds all joints' bindpose matrices.
	Skeleton gSkeleton;

	// Animation class which holds keyframes.
	Animation** gAnimations;
	unsigned int gAnimationCount;
	Animation* getAnimation(const char* animName);

	// Graphics
	bool LoadFBX(const char* fileName);

	// Constant buffers on GPU side
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	bool hasGPUBuffers = false;

	// Shapes
	void createBoxGeometry(Vector3 gSize);
};