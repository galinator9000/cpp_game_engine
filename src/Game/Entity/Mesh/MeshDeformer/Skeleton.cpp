#include "Skeleton.h"

void Skeleton::Setup() {
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
}

void Skeleton::Update() {

}

Joint* Skeleton::getRootJoint() {
	for (unsigned int j = 0; j < this->gJointCount; j++) {
		if (this->gJoints[j]->isRoot) {
			return this->gJoints[j];
		}
	}
	return NULL;
}