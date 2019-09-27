#include "Joint.h"

Joint::Joint(int id, std::string name) {
	this->id = id;
	this->name = name;

	// Initialize matrices with identity matrix.
	dx::XMStoreFloat4x4(&this->globalBindPoseMatrix, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->globalBindPoseInverseMatrix, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->jointLocalBindTransform, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->jointLocalBindTransformInverse, dx::XMMatrixIdentity());
}