#include "MeshDeformer.h"

void MeshDeformer::Update() {
	for (unsigned int j = 0; j < this->gJointCount; j++) {
		// Fill transform matrices that will be provided to Vertex Shader.
		if (this->gJoints[j].dataChanged) {
			this->gMeshDeformerVSConstantBuffer.jointsTransformMatrix[this->gJoints[j].id] = this->gJoints[j].jointMatrix;

			this->shouldUpdateGPUData = true;
			this->gJoints[j].dataChanged = false;
		}
	}
}

bool MeshDeformer::calculateMatrices() {
	dx::XMFLOAT4X4 identityMatrix;
	dx::XMStoreFloat4x4(&identityMatrix, dx::XMMatrixIdentity());

	// Calculate all toRootSpaceMatrix on every joint recursively.
	// Root joint doesn't have any parent so we just pass identity matrix.
	calculateToRootMatrices(
		getRootJoint(),
		identityMatrix
	);

	return true;
}

bool MeshDeformer::calculateToRootMatrices(Joint* baseJoint, dx::XMFLOAT4X4 parentToRootMatrix) {
	dx::XMStoreFloat4x4(
		&baseJoint->toRootSpaceMatrix,
		dx::XMLoadFloat4x4(&baseJoint->toParentSpaceMatrix) * dx::XMLoadFloat4x4(&parentToRootMatrix)
	);

	for (unsigned int j = 0; j < baseJoint->childJoints.size(); j++) {
		calculateToRootMatrices(
			baseJoint->childJoints[j],
			baseJoint->toRootSpaceMatrix
		);
	}
	return true;
}

// General
Joint* MeshDeformer::getRootJoint() {
	for (unsigned int j = 0; j < this->gJointCount; j++) {
		if (this->gJoints[j].isRoot) {
			return &(this->gJoints[j]);
		}
	}
	return NULL;
}