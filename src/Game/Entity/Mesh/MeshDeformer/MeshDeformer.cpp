#include "MeshDeformer.h"

MeshDeformer::MeshDeformer(){}

void MeshDeformer::Setup() {
	// Create JointTransform object array.
	this->gJointTransforms = new JointTransform *[this->skeleton->gJointCount];
	this->gJointCount = this->skeleton->gJointCount;

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
}

void MeshDeformer::Update() {
	for (unsigned int j = 0; j < this->skeleton->gJointCount; j++) {
		// If animation attached, apply current pose to joints.
		if (this->isAnimating) {
			this->gAnimator->Update(this->gJointTransforms, this->gJointCount);
		}

		if (this->gJointTransforms[j]->dataChanged) {
			this->gMeshDeformerVSConstantBuffer.jointsTransformMatrix[this->skeleton->gJoints[j]->id] = this->gJointTransforms[j]->jointModelTransformMatrix;
			this->shouldUpdateGPUData = true;
			this->gJointTransforms[j]->dataChanged = false;
		}
	}

	this->rootInitialMatrix = dx::XMMatrixIdentity();
	// Recalculate joint matrices.
	this->recalculateMatrices(
		this->rootJointID,
		&this->rootInitialMatrix
	);
}

// Recalculates joint matrices recursively.
void MeshDeformer::recalculateMatrices(int baseJointID, dx::XMMATRIX* parentModelTransform) {
	// Calculate current joint's transform in model-space.
	Joint* baseJoint = this->skeleton->gJoints[baseJointID];
	JointTransform* baseJointTransform = this->gJointTransforms[baseJointID];

	dx::XMMATRIX poseModelTransformMatrix;
	if (this->isAnimating) {
		poseModelTransformMatrix = dx::XMLoadFloat4x4(&baseJointTransform->jointAnimTransformMatrix) * (*parentModelTransform);
	}
	else {
		poseModelTransformMatrix = dx::XMLoadFloat4x4(&baseJointTransform->jointLocalTransformMatrix) * dx::XMLoadFloat4x4(&baseJoint->jointLocalBindTransform) * (*parentModelTransform);
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