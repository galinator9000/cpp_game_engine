#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <algorithm>

#include <Timer.h>
#include "Animation/Animation.h"
#include "Structs.h"

namespace wrl = Microsoft::WRL;

class MeshDeformer{
public:
	void Setup();
	void Update();
	void setAnimation(Animation* animation);
	void recalculateMatrices(int baseJointID, dx::XMMATRIX* jointLocalTransform);

	// Skeleton class which holds all joints' bindpose matrices.
	// This pointer is given by entity while attaching deformer.
	Skeleton* skeleton;

	// Animation object that holds keyframes that holds each joint's transformation for the current pose.
	Animation* currentAnimation = NULL;
	unsigned int currentKeyframeIndex = 0;
	bool isAnimating = false;
	Timer timer;

	// Joint transforms for each joint on skeleton.
	// Remember, skeleton object is holding bind pose of the joints.
	// This objects will define pose of the joints.
	// And should exactly match with Skeleton's gJoints array.
	JointTransform** gJointTransforms;
	unsigned int gJointCount;
	int rootJointID;
	dx::XMMATRIX rootInitialMatrix;

	// Constant buffer
	MeshDeformerVSConstantBuffer gMeshDeformerVSConstantBuffer;
	wrl::ComPtr<ID3D11Buffer> pMeshDeformerVSConstantBuffer;
	bool shouldUpdateGPUData = false;
	bool dataChanged = false;
};