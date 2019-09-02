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
	void updateConstantBuffer();

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