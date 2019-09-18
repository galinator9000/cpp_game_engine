#pragma once
#include "Joint.h"

#include <DirectXMath.h>
#include <string>
#include <vector>
#include <map>
#include <math.h>

namespace dx = DirectX;

class JointTransform{
public:
	JointTransform();

	int id = -1;
	std::string name = "";
	bool isRoot = false;
	bool dataChanged = true;

	// Transformation matrix that will be provided to Vertex shader and will affect vertices which this joint influences.
	dx::XMFLOAT4X4 jointModelTransformMatrix;

	// Transformation matrices that defines transform of the joint relative to parent joint.
	// One used for animation, other is for animating it manually or physics.
	dx::XMFLOAT4X4 jointAnimTransformMatrix;
	dx::XMFLOAT4X4 jointLocalTransformMatrix;
};