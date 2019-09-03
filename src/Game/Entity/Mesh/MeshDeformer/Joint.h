#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <map>
#include <math.h>

namespace dx = DirectX;

// Each joint on mesh deformer is represented by this class.
class Joint {
public:
	Joint(int id, std::string name);
	void Update();

	int id = -1;
	std::string name;
	bool isRoot = false;
	bool dataChanged = true;

	// Hierarchy.
	int parentJointID = -1;
	Joint* parentJoint = NULL;
	std::vector<int> childJointIDs;
	std::vector<Joint*> childJoints;

	// Transformation matrix that will affect vertices which this joint influences.
	dx::XMFLOAT4X4 jointMatrix;
	dx::XMFLOAT4X4 jointTransformMatrix;

	// Matrices that comes from FBX SDK
	dx::XMFLOAT4X4 toModelSpace;
	dx::XMFLOAT4X4 toJointSpace;

	// Position and rotation values of the joint,
	// these values are relative to joint's pivot point.
	dx::XMFLOAT3 position;
	dx::XMFLOAT4 rotationQ;
};