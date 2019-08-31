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

	// World -> JointLocal matrix.
	dx::XMFLOAT4X4* transformMatrix;

	// JointLocal -> World matrix.
	dx::XMFLOAT4X4* transformLinkMatrix;

	// World bind pose matrix.
	dx::XMFLOAT4X4* globalBindposeInverseMatrix;

	Joint* parentJoint;
	std::vector<Joint*> childJoints;

	std::map<int, double> vertexIndexWeightPair;
};