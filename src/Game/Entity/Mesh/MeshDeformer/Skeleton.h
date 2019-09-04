#pragma once

#include "Joint.h"

class Skeleton{
public:
	void Setup();
	void Update();

	// Joints
	Joint** gJoints;
	unsigned int gJointCount;

	Joint* rootJoint;
	Joint* getRootJoint();
};