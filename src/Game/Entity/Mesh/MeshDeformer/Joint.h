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

	dx::XMFLOAT4X4* transformMatrix;
	dx::XMFLOAT4X4* transformLinkMatrix;

	Joint* parentJoint;
	std::vector<Joint*> childJoints;

	std::map<int, double> controlPointIndexWeightPair;
};