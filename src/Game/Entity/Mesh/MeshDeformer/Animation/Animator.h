#pragma once
#include "Animation.h"
#include "Core/Timer.h"
#include "Entity/Mesh/MeshDeformer/JointTransform.h"

class Animator{
public:
	void Update(JointTransform** jointTransforms, unsigned int gJointCount);
	void setAnimation(Animation* animation);

	// Animation object that holds keyframes that holds each joint's transformation for the current pose.
	Animation* currentAnimation = NULL;
	Timer timer;

	// Transformation matrix interpolation.
	// Progression parameter is basically weighting while blending transformation matrices.
	dx::XMFLOAT4X4 interpolateTransformationMatrix(dx::XMFLOAT4X4 matrix1, dx::XMFLOAT4X4 matrix2, float progression);
};