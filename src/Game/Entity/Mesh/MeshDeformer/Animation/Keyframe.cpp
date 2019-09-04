#include "Keyframe.h"

Keyframe::Keyframe(float timeStamp) {
	this->timeStamp = timeStamp;
}

void Keyframe::setJointKeyframeMatrix(unsigned int jointID, dx::XMFLOAT4X4* jointTransform) {
	this->jointIDTransform[jointID] = jointTransform;
}