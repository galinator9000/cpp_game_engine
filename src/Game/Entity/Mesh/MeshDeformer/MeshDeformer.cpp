#include "MeshDeformer.h"

MeshDeformer::MeshDeformer(bool useRagdoll){
	this->useRagdoll = useRagdoll;
}

void MeshDeformer::Setup(Skeleton* pSkeleton, dx::XMFLOAT3 gEntityScale) {
	this->skeleton = pSkeleton;

	// Create JointTransform object array.
	this->gJointCount = this->skeleton->gJointCount;
	this->gJointTransforms = new JointTransform*[this->gJointCount];

	// Fill JointTransform objects data.
	for (unsigned int j = 0; j < this->gJointCount; j++) {
		// Copy joint values from skeleton.
		this->gJointTransforms[j] = new JointTransform();
		this->gJointTransforms[j]->id = this->skeleton->gJoints[j]->id;
		this->gJointTransforms[j]->name = this->skeleton->gJoints[j]->name;
		this->gJointTransforms[j]->isRoot = this->skeleton->gJoints[j]->isRoot;
	}

	this->rootJointID = this->skeleton->rootJoint->id;

	// Set root joint's initial matrix to identity matrix.
	this->rootInitialMatrix = dx::XMMatrixIdentity();

	// Setup ragdoll joints on physics side if used.
	if (this->useRagdoll) {
		// Create physical actor&shape arrays.
		this->pRagdollCollisionActor = new CollisionActor*[this->gJointCount];
		this->pRagdollCollisionShape = new CollisionShape*[this->gJointCount];

		for (unsigned int j = 0; j < this->gJointCount; j++) {
			// Fill physical actor&shape arrays.
			this->pRagdollCollisionActor[j] = new CollisionActor(COLLISION_ACTOR_DYNAMIC);
			this->pRagdollCollisionShape[j] = new CollisionShape();

			Joint* currentJoint = this->skeleton->gJoints[j];
			JointTransform* currentJointTransform = this->gJointTransforms[j];

			// Root joint.
			if (currentJoint->isRoot) {
				dx::XMStoreFloat3(
					&(this->rootJointAbsolutePos),
					XMVector3Transform(
						dx::XMVectorZero(),
						dx::XMLoadFloat4x4(&(currentJoint->globalBindPoseMatrix))
					)
				);

				/*this->rootJointAbsolutePos.x *= gEntityScale.x;
				this->rootJointAbsolutePos.y *= gEntityScale.y;
				this->rootJointAbsolutePos.z *= gEntityScale.z;*/

				this->pRagdollCollisionShape[j]->createBoxGeometry({ 1, 1, 1 });
				this->pRagdollCollisionActor[j]->initialTransform = PxTransform(
					PxVec3(
						this->rootJointAbsolutePos.x,
						this->rootJointAbsolutePos.y,
						this->rootJointAbsolutePos.z
					)
				);

				this->pRagdollCollisionActor[j]->parentActor = NULL;

				continue;
			}

			this->pRagdollCollisionActor[j]->parentActor = this->pRagdollCollisionActor[currentJoint->parentJointID];

			dx::XMFLOAT3 jointAbsolutePos;
			dx::XMStoreFloat3(
				&jointAbsolutePos,
				XMVector3Transform(
					dx::XMVectorZero(),
					dx::XMLoadFloat4x4(&(currentJoint->globalBindPoseMatrix))
				)
			);

			dx::XMFLOAT3 parentJointAbsolutePos;
			dx::XMStoreFloat3(
				&parentJointAbsolutePos,
				XMVector3Transform(
					dx::XMVectorZero(),
					dx::XMLoadFloat4x4(&(currentJoint->parentJoint->globalBindPoseMatrix))
				)
			);

			// Scale.
			/*jointAbsolutePos.x *= gEntityScale.x;
			jointAbsolutePos.y *= gEntityScale.y;
			jointAbsolutePos.z *= gEntityScale.z;
			parentJointAbsolutePos.x *= gEntityScale.x;
			parentJointAbsolutePos.y *= gEntityScale.y;
			parentJointAbsolutePos.z *= gEntityScale.z;*/

			// Measure length
			dx::XMFLOAT3 jointLength(
				parentJointAbsolutePos.x - jointAbsolutePos.x,
				parentJointAbsolutePos.y - jointAbsolutePos.y,
				parentJointAbsolutePos.z - jointAbsolutePos.z
			);
			dx::XMStoreFloat3(
				&jointLength,
				dx::XMVector3Length(dx::XMLoadFloat3(&jointLength))
			);

			this->pRagdollCollisionShape[j]->createBoxGeometry({ 1, jointLength.x, 1 });
			this->pRagdollCollisionActor[j]->initialTransform = PxTransform(
				PxVec3(
					parentJointAbsolutePos.x + (jointAbsolutePos.x - parentJointAbsolutePos.x) / 2,
					parentJointAbsolutePos.y + (jointAbsolutePos.y - parentJointAbsolutePos.y) / 2,
					parentJointAbsolutePos.z + (jointAbsolutePos.z - parentJointAbsolutePos.z) / 2
				)
			);
		}
	}
}

void MeshDeformer::Update() {
	// If animation attached, apply current pose of animation to joints.
	if (this->isAnimating) {
		this->gAnimator->Update(this->gJointTransforms, this->gJointCount);
	}
	// If ragdoll physics is activated, update accordingly.
	else if (this->isRagdollActive) {

	}

	// Recalculate joint matrices.
	this->recalculateMatrices(
		this->rootJointID,
		&this->rootInitialMatrix
	);

	for (unsigned int j = 0; j < this->gJointCount; j++) {
		if (this->gJointTransforms[j]->dataChanged) {
			this->gMeshDeformerVSConstantBuffer.jointsTransformMatrix[this->skeleton->gJoints[j]->id] = this->gJointTransforms[j]->jointModelTransformMatrix;
			this->shouldUpdateGPUData = true;
			this->gJointTransforms[j]->dataChanged = false;
		}
	}
}

// Recalculates joint matrices recursively.
void MeshDeformer::recalculateMatrices(int baseJointID, dx::XMMATRIX* parentModelTransform) {
	// Calculate current joint's transform in model-space.
	Joint* baseJoint = this->skeleton->gJoints[baseJointID];
	JointTransform* baseJointTransform = this->gJointTransforms[baseJointID];

	dx::XMMATRIX poseModelTransformMatrix = dx::XMLoadFloat4x4(&baseJoint->globalBindPoseMatrix);
	if (this->isAnimating) {
		poseModelTransformMatrix = (
			// Transformation in global-space.
			//dx::XMLoadFloat4x4(&baseJointTransform->jointAnimTransformMatrix)

			// Transformation relative to parent.
			dx::XMLoadFloat4x4(&baseJointTransform->jointAnimTransformMatrix) *
			(*parentModelTransform)
		);
	}
	else if (this->isRagdollActive) {
		// Transformation relative to parent.
		poseModelTransformMatrix = (
			dx::XMLoadFloat4x4(&baseJointTransform->jointLocalTransformMatrix) *
			(*parentModelTransform)
		);
	}
	else {
		// Transformation relative to parent.
		poseModelTransformMatrix = (
			dx::XMLoadFloat4x4(&baseJointTransform->jointLocalTransformMatrix) *
			dx::XMLoadFloat4x4(&baseJoint->jointLocalBindTransform) *
			(*parentModelTransform)
		);
	}
	
	// Iterate over child joints and recalculate their matrices too.
	for (int cj = 0; cj < baseJoint->childJoints.size(); cj++) {
		this->recalculateMatrices(
			baseJoint->childJoints.at(cj)->id,
			&poseModelTransformMatrix
		);
	}

	// Update final matrix that will be applied to vertices.
	dx::XMStoreFloat4x4(
		&baseJointTransform->jointModelTransformMatrix,
		dx::XMMatrixTranspose(
			dx::XMLoadFloat4x4(&baseJoint->globalBindPoseInverseMatrix) * poseModelTransformMatrix
		)
	);
	baseJointTransform->dataChanged = true;
}

void MeshDeformer::setAnimation(Animation* animation) {
	// If animator isn't created yet, create it.
	if (animation == NULL) {
		return;
	}

	if (this->gAnimator == NULL) {
		this->gAnimator = new Animator();
	}

	this->gAnimator->setAnimation(animation);
	this->isAnimating = true;
}

// Ragdoll physics
void MeshDeformer::activateRagdoll() {
	this->useRagdoll = true;
}

void MeshDeformer::deactivateRagdoll() {
	this->useRagdoll = false;
}