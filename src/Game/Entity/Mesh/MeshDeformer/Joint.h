#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <map>

namespace dx = DirectX;

// Each joint on mesh deformer is represented by this class.
class Joint {
public:
	unsigned int id;
	std::string name;
	bool isRoot = false;
	bool dataChanged = false;

	// Transformation matrix that will be provided to Vertex Shader for
	// transforming related vertex relative to this joint's transformation.
	dx::XMFLOAT4X4* transformMatrixVS;

	// JointLocal -> World matrix.
	dx::XMFLOAT4X4* transformMatrix;
	dx::XMFLOAT4X4* transformLinkMatrix;

	// Inverse global bind pose matrix.
	dx::XMFLOAT4X4* globalBindposeInverseMatrix;

	Joint* parentJoint;
	std::vector<Joint*> childJoints;
};