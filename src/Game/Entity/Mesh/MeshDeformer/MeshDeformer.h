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
	void Update();
	bool calculateMatrices();
	bool calculateToRootMatrices(Joint* baseJoint, dx::XMFLOAT4X4 parent_ToRootMatrix);

	// Joints
	Joint* gJoints;
	unsigned int gJointCount;
	Joint* getRootJoint();

	// Constant buffer
	MeshDeformerVSConstantBuffer gMeshDeformerVSConstantBuffer;
	wrl::ComPtr<ID3D11Buffer> pMeshDeformerVSConstantBuffer;
	bool shouldUpdateGPUData = false;
	bool dataChanged = false;
};