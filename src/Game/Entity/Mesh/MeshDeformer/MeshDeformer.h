#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <algorithm>

#include "Structs.h"

class MeshDeformer{
public:
	void Update();

	// Joints
	Joint* gJoints;
	unsigned int gJointCount;
};