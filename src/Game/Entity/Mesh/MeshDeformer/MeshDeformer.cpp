#include "MeshDeformer.h"

void MeshDeformer::Update() {
	for (unsigned int j = 0; j < this->gJointCount; j++) {
		// Fill transform matrices that will be provided to Vertex Shader.
		if (this->gJoints[j].dataChanged) {
			this->gMeshDeformerVSConstantBuffer.jointsTransformMatrix[this->gJoints[j].id] = (*this->gJoints[j].transformMatrixVS);
			this->shouldUpdateGPUData = true;
		}
	}
}

Joint* MeshDeformer::getRootJoint() {
	for (unsigned int j = 0; j < this->gJointCount; j++) {
		if (this->gJoints[j].isRoot) {
			return &(this->gJoints[j]);
		}
	}
	return NULL;
}

bool MeshDeformer::calculateMatrices() {
	//calculateToRootMatrices();
	return true;
}

/*bool MeshDeformer::calculateToRootMatrices() {
	Joint* rootJoint = this->getRootJoint();

	if (rootJoint == NULL) {
		return false;
	}

	dx::XMFLOAT4X4* identityMatrix = new dx::XMFLOAT4X4();
	dx::XMStoreFloat4x4(
		identityMatrix,
		dx::XMMatrixIdentity()
	);

	calculateToRootMatrixRecursive(
		rootJoint,
		identityMatrix
	);
	return true;
}

void MeshDeformer::calculateToRootMatrixRecursive(Joint* joint, dx::XMFLOAT4X4* parentMatrix) {
	joint->transformLinkMatrix;

	for (unsigned int j = 0; j < joint->childJoints.size(); j++) {
		calculateToRootMatrixRecursive(
			joint->childJoints[j],
			joint->transformMatrix
		);
	}
}*/