#include "MeshDeformer.h"

void MeshDeformer::Setup() {
	// Recalculate joint matrices.
	// Obviously, root joint has no parent, we just pass identity matrix.
	this->recalculateMatrices(this->skeleton->rootJoint, &dx::XMMatrixIdentity());
}

void MeshDeformer::Update() {
	for (unsigned int j = 0; j < this->skeleton->gJointCount; j++) {
		// Fill transform matrices that will be provided to Vertex Shader.
		this->skeleton->gJoints[j]->Update();

		if (this->skeleton->gJoints[j]->dataChanged) {
			this->gMeshDeformerVSConstantBuffer.jointsTransformMatrix[this->skeleton->gJoints[j]->id] = this->skeleton->gJoints[j]->jointModelTransformMatrix;
			this->shouldUpdateGPUData = true;
			this->skeleton->gJoints[j]->dataChanged = false;
		}
	}

	// Recalculate joint matrices.
	// Obviously, root joint has no parent, we just pass identity matrix.
	this->recalculateMatrices(this->skeleton->rootJoint, &dx::XMMatrixIdentity());
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