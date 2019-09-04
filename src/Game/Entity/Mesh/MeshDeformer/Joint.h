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

	int id = -1;
	std::string name;
	bool isRoot = false;

	// Hierarchy.
	int parentJointID = -1;
	Joint* parentJoint = NULL;
	std::vector<int> childJointIDs;
	std::vector<Joint*> childJoints;

	//// Matrices that comes from FBX SDK
	dx::XMFLOAT4X4 jointBindPoseInverseMatrix;
	dx::XMFLOAT4X4 jointLocalBindTransform;
};