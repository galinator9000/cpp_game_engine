#include "MeshDeformer.h"

void MeshDeformer::Setup() {
	// Create JointTransform object array.
	this->gJointTransforms = new JointTransform * [this->skeleton->gJointCount];

	// Fill JointTransform objects data.
	for (unsigned int j = 0; j < this->skeleton->gJointCount; j++) {
		// Copy joint values from skeleton.
		this->gJointTransforms[j] = new JointTransform();
		this->gJointTransforms[j]->id = this->skeleton->gJoints[j]->id;
		this->gJointTransforms[j]->name = this->skeleton->gJoints[j]->name;
		this->gJointTransforms[j]->isRoot = this->skeleton->gJoints[j]->isRoot;
	}

	// Recalculate joint matrices.
	// Obviously, root joint has no parent, we just pass identity matrix.
	this->rootJointID = this->skeleton->rootJoint->id;
	this->recalculateMatrices(this->rootJointID, &dx::XMMatrixIdentity());
}

void MeshDeformer::Update() {
	// Keep animation progressing with usage of Timer.
	if (this->currentAnimation != NULL && this->isAnimating) {
		if (this->timer.Peek() > currentAnimation->duration) {
			this->timer.Reset();
		}
		this->currentKeyframeIndex = (unsigned int)(this->timer.Peek() / currentAnimation->duration * currentAnimation->gKeyframeCount);

		if (this->currentKeyframeIndex >= currentAnimation->gKeyframeCount) {
			this->currentKeyframeIndex = currentAnimation->gKeyframeCount - 1;
		}
	}

	for (unsigned int j = 0; j < this->skeleton->gJointCount; j++) {
		// If animation attached, apply current pose to joints.
		if (this->currentAnimation != NULL && this->isAnimating) {
			this->gJointTransforms[j]->jointLocalTransformMatrix = *(this->currentAnimation->gKeyFrames[this->currentKeyframeIndex]->jointIDTransform[j]);
		}
		// Otherwise, apply arbitrary rotation.
		else {
			if (
				!this->gJointTransforms[j]->name.compare("spine")
				) {
				if (this->timer.Peek() > 1.0f) {
					this->timer.Reset();
				}

				float x, y, z;
				x = 0;
				y = 0;
				z = 1;

				dx::XMStoreFloat4x4(
					&this->gJointTransforms[j]->jointLocalTransformMatrix,
					dx::XMMatrixRotationQuaternion(
						dx::XMQuaternionRotationAxis(
							dx::XMLoadFloat3(
								&dx::XMFLOAT3(
									x, y, z
								)
							),
							this->timer.Peek() * 6.28f
						)
					)
				);
			}
		}

		if (this->gJointTransforms[j]->dataChanged) {
			this->gMeshDeformerVSConstantBuffer.jointsTransformMatrix[this->skeleton->gJoints[j]->id] = this->gJointTransforms[j]->jointModelTransformMatrix;
			this->shouldUpdateGPUData = true;
			this->gJointTransforms[j]->dataChanged = false;
		}
	}

	// Recalculate joint matrices.
	this->recalculateMatrices(this->rootJointID, &dx::XMMatrixIdentity());
}

// Recalculates joint matrices recursively.
void MeshDeformer::recalculateMatrices(int baseJointID, dx::XMMATRIX* parentModelTransform) {
	// Calculate current joint's transform in model-space.
	Joint* baseJoint = this->skeleton->gJoints[baseJointID];
	JointTransform* baseJointTransform = this->gJointTransforms[baseJointID];

	dx::XMMATRIX poseModelTransformMatrix = dx::XMLoadFloat4x4(&baseJointTransform->jointLocalTransformMatrix) * dx::XMLoadFloat4x4(&baseJoint->jointLocalBindTransform) * (*parentModelTransform);

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
	this->currentAnimation = animation;
	this->isAnimating = true;
}