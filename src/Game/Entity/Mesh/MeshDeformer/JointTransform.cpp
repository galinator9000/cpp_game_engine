#include "JointTransform.h"

JointTransform::JointTransform() {
	// Initialize translation and rotation vectors.
	this->position = dx::XMFLOAT3(0, 0, 0);
	this->rotationQ = dx::XMFLOAT4(0, 0, 0, 0);

	// Initialize matrices with identity matrix.
	dx::XMStoreFloat4x4(&this->jointModelTransformMatrix, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->jointLocalTransformMatrix, dx::XMMatrixIdentity());
}