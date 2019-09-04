#include "Joint.h"

Joint::Joint(int id, std::string name) {
	this->id = id;
	this->name = name;

	// Initialize matrices with identity matrix.
	dx::XMStoreFloat4x4(&this->jointBindPoseInverseMatrix, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->jointLocalBindTransform, dx::XMMatrixTranslation(0, 0, 0));
}