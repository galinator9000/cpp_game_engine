#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <algorithm>

#include "Structs.h"

namespace wrl = Microsoft::WRL;

class MeshDeformer{
public:
	void Setup();
	void Update();
	void recalculateMatrices(int baseJointID, dx::XMMATRIX* jointLocalTransform);

	// Skeleton class which holds all joints' bindpose matrices.
	// This pointer is given by entity while attaching deformer.
	Skeleton* skeleton;

	// Joint transforms for each joint on skeleton.
	// Remember, skeleton object is holding bind pose of the joints.
	// This objects will define pose of the joints.
	// And should exactly match with Skeleton's gJoints array.
	JointTransform* gJointTransforms;
	unsigned int gJointCount;
	int rootJointID;

	// Constant buffer
	MeshDeformerVSConstantBuffer gMeshDeformerVSConstantBuffer;
	wrl::ComPtr<ID3D11Buffer> pMeshDeformerVSConstantBuffer;
	bool shouldUpdateGPUData = false;
	bool dataChanged = false;
};