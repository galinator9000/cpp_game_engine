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
}

void MeshDeformer::Update() {
	for (unsigned int j = 0; j < this->gJointCount; j++) {
		// Fill transform matrices that will be provided to Vertex Shader.
		this->gJoints[j]->Update();

		if (this->gJoints[j]->dataChanged) {
			this->gMeshDeformerVSConstantBuffer.jointsTransformMatrix[this->gJoints[j]->id] = this->gJoints[j]->jointMatrix;
			this->shouldUpdateGPUData = true;
			this->gJoints[j]->dataChanged = false;
		}
	}
}

// General
Joint* MeshDeformer::getRootJoint() {
	for (unsigned int j = 0; j < this->gJointCount; j++) {
		if (this->gJoints[j]->isRoot) {
			return this->gJoints[j];
		}
	}
	return NULL;
}