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
	void Update();

	std::string name;
	bool dataChanged = true;

	// Transformation matrix that will be provided to Vertex shader and will affect vertices which this joint influences.
	dx::XMFLOAT4X4 jointModelTransformMatrix;
	// Transformation matrix that defines transform of the joint relative to parent joint.
	dx::XMFLOAT4X4 jointLocalTransformMatrix;

	// Position and rotation values of the joint,
	// these values are relative to joint's pivot point.
	dx::XMFLOAT3 position;
	dx::XMFLOAT4 rotationQ;
};