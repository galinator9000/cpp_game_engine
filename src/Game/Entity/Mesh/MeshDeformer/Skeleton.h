#pragma once
////// Represents joint hierarchy IN BIND POSE of the mesh.

#include "Joint.h"

class Skeleton{
public:
	void Setup();

	// Joints
	Joint** gJoints;
	unsigned int gJointCount;

	Joint* rootJoint;
	Joint* getRootJoint();
};