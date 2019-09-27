#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <algorithm>

#include "Animation/Animator.h"
#include "Entity/Collision/CollisionActor.h"
#include "Entity/Collision/CollisionShape.h"
#include "Structs.h"
#include "Timer.h"

namespace wrl = Microsoft::WRL;

enum JOINT_TRANSFORM_SOURCE {
	NONE,
	ANIMATION,
	RAGDOLL
};

class MeshDeformer{
public:
	MeshDeformer();
	void Setup(Skeleton* pSkeleton, dx::XMFLOAT3 gEntityScale);
	void Update();
	void recalculateMatrices(int baseJointID, dx::XMMATRIX* jointLocalTransform);

	// Skeleton class which holds all joints' bindpose matrices.
	// This pointer is given by entity while attaching deformer.
	Skeleton* skeleton;
	
	JOINT_TRANSFORM_SOURCE currentTransformSource = NONE;

	// Ragdoll physics
	CollisionActor** pRagdollCollisionActor;
	CollisionShape** pRagdollCollisionShape;
	void activateRagdoll();

	// Animator class which takes keyframes, interpolates between them and sends matrices back.
	Animator* gAnimator = NULL;
	void setAnimation(Animation* animation);

	// Joint transforms for each joint on skeleton.
	// Remember, skeleton object is holding bind pose of the joints.
	// This objects will define pose of the joints.
	// And should exactly match with Skeleton's gJoints array.
	JointTransform** gJointTransforms;
	unsigned int gJointCount = 0;
	int rootJointID = -1;
	dx::XMMATRIX rootInitialMatrix;

	// Constant buffer
	MeshDeformerVSConstantBuffer gMeshDeformerVSConstantBuffer;
	wrl::ComPtr<ID3D11Buffer> pMeshDeformerVSConstantBuffer;
	bool shouldUpdateGPUData = false;
	bool dataChanged = false;

	Timer timer;
};