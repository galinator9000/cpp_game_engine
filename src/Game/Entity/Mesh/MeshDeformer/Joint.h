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
	void Update();

	unsigned int id;
	std::string name;
	bool isRoot = false;
	bool dataChanged = true;

	// Transformation matrix that will affect vertices which this joint influences.
	dx::XMFLOAT4X4 jointMatrix;

	dx::XMFLOAT4X4 worldMatrix;
	dx::XMFLOAT4X4 inverseWorldMatrix;

	Joint* parentJoint;
	std::vector<Joint*> childJoints;
};