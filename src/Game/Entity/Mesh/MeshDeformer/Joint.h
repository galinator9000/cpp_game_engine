#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <map>

namespace dx = DirectX;

// Each joint on mesh deformer is represented by this class.
class Joint {
public:
	Joint();
	unsigned int id;
	std::string name;
	bool isRoot = false;
	bool dataChanged = true;

	// Transformation matrix that transforms from joint's space to model's space.
	dx::XMFLOAT4X4 jointMatrix;

	// Transformation matrix that transforms from joint's space to parent joint's space.
	dx::XMFLOAT4X4 toParentSpaceMatrix;

	// Transformation matrix that transforms from joint's space to root joint's space.
	dx::XMFLOAT4X4 toRootSpaceMatrix;

	dx::XMFLOAT4X4 transformMatrix;
	dx::XMFLOAT4X4 transformLinkMatrix;

	Joint* parentJoint;
	std::vector<Joint*> childJoints;
};