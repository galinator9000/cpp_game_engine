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
	Joint* getRootJoint();
	bool calculateMatrices();

	// Joints
	Joint* gJoints;
	unsigned int gJointCount;

	// Constant buffer
	MeshDeformerVSConstantBuffer gMeshDeformerVSConstantBuffer;
	wrl::ComPtr<ID3D11Buffer> pMeshDeformerVSConstantBuffer;
};