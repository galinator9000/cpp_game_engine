#include "Joint.h"

Joint::Joint() {
	// Initialize matrices with identity matrix.
	dx::XMStoreFloat4x4(&this->jointMatrix, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->worldMatrix, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->inverseWorldMatrix, dx::XMMatrixIdentity());
}

void Joint::Update() {
	// Pick random joint.
	if(false){
	//if (std::string(this->name).compare("upper_arm.L") == 0) {
		// Build arbitrary transformation matrix.
		float x = 1.57f;
		float y = 0.0f;
		float z = 0.0f;

		dx::XMMATRIX localMatrix = dx::XMMatrixRotationQuaternion(
			dx::XMQuaternionRotationAxis(
				dx::XMLoadFloat3(
					&dx::XMFLOAT3(
						x, y, z
					)
				),
				1.0f
			)
		);

		// Update world matrix.
		if (this->isRoot) {
			dx::XMStoreFloat4x4(
				&this->worldMatrix,
				localMatrix
			);
		}
		else {
			dx::XMStoreFloat4x4(
				&this->worldMatrix,
				dx::XMLoadFloat4x4(&this->parentJoint->worldMatrix) * localMatrix
			);
		}

		// Update final transformation matrix.
		dx::XMStoreFloat4x4(
			&this->jointMatrix,
			dx::XMLoadFloat4x4(&this->inverseWorldMatrix) * dx::XMLoadFloat4x4(&this->worldMatrix)
		);
		this->dataChanged = true;
	}
}