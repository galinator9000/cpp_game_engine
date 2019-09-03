#include "MeshDeformer.h"

void MeshDeformer::Setup() {
	for (unsigned int j = 0; j < this->gJointCount; j++) {
		// Attach parent joint pointer.
		if (this->gJoints[j]->parentJointID != -1) {
			this->gJoints[j]->parentJoint = this->gJoints[this->gJoints[j]->parentJointID];
		}

		// Attach child joint pointers.
		for (unsigned int cj = 0; cj < this->gJoints[j]->childJointIDs.size(); cj++) {
			int childJointId = this->gJoints[j]->childJointIDs.at(cj);
			this->gJoints[j]->childJoints.push_back(this->gJoints[childJointId]);
		}
	}

	// Record root joint.
	this->rootJoint = this->getRootJoint();

	// Recalculate joint matrices.
	// Obviously, root joint has no parent, we just pass identity matrix.
	this->recalculateMatrices(this->rootJoint, &dx::XMMatrixIdentity());
}

void MeshDeformer::Update() {
	for (unsigned int j = 0; j < this->gJointCount; j++) {
		// Fill transform matrices that will be provided to Vertex Shader.
		this->gJoints[j]->Update();

		if (this->gJoints[j]->dataChanged) {
			this->gMeshDeformerVSConstantBuffer.jointsTransformMatrix[this->gJoints[j]->id] = this->gJoints[j]->jointModelTransformMatrix;
			this->shouldUpdateGPUData = true;
			this->gJoints[j]->dataChanged = false;
		}
	}

	// Recalculate joint matrices.
	// Obviously, root joint has no parent, we just pass identity matrix.
	this->recalculateMatrices(this->rootJoint, &dx::XMMatrixIdentity());
}

// Recalculates joint matrices recursively.
void MeshDeformer::recalculateMatrices(Joint* baseJoint, dx::XMMATRIX* parentModelTransform) {
	// Calculate current joint's transform in model-space.
	dx::XMMATRIX poseModelTransformMatrix = dx::XMLoadFloat4x4(&baseJoint->jointLocalTransformMatrix) * dx::XMLoadFloat4x4(&baseJoint->jointLocalBindTransform) * (*parentModelTransform);

	// Iterate over child joints and recalculate their matrices too.
	for (int cj = 0; cj < baseJoint->childJoints.size(); cj++) {
		MeshDeformer::recalculateMatrices(
			baseJoint->childJoints.at(cj),
			&poseModelTransformMatrix
		);
	}

	// Update final matrix that will be applied to vertices.
	dx::XMStoreFloat4x4(
		&baseJoint->jointModelTransformMatrix,
		dx::XMMatrixTranspose(
			dx::XMLoadFloat4x4(&baseJoint->jointBindPoseInverseMatrix) * poseModelTransformMatrix
		)
	);
	baseJoint->dataChanged = true;
}

Joint* MeshDeformer::getRootJoint() {
	for (unsigned int j = 0; j < this->gJointCount; j++) {
		if (this->gJoints[j]->isRoot) {
			return this->gJoints[j];
		}
	}
	return NULL;
}