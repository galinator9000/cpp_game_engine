#pragma once
#include "Structs.h"

#include <DirectXMath.h>
#include <string>
#include <vector>
#include <map>
#include <math.h>

namespace dx = DirectX;

class Keyframe{
public:
	Keyframe(float timeStamp);
	void setJointKeyframeMatrix(unsigned int jointID, dx::XMFLOAT4X4* jointTransform);

	float timeStamp;

	std::map<int, dx::XMFLOAT4X4*> jointIDTransform;
};