#include "MeshDeformer.h"

void MeshDeformer::Update() {
	for (unsigned int j = 0; j < this->gJointCount; j++) {
		// Fill transform matrices that will be provided to Vertex Shader.
		this->gJoints[j].Update();

		if (this->gJoints[j].dataChanged) {
			this->gMeshDeformerVSConstantBuffer.jointsTransformMatrix[this->gJoints[j].id] = this->gJoints[j].jointMatrix;
			this->shouldUpdateGPUData = true;
			this->gJoints[j].dataChanged = false;
		}
	}
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