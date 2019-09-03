#include "Joint.h"

Joint::Joint() {
	// Initialize translation and rotation vectors.
	this->position = dx::XMFLOAT3(0, 0, 0);
	this->rotationQ = dx::XMFLOAT4(0, 0, 0, 0);

	// Initialize matrices with identity matrix.
	dx::XMStoreFloat4x4(&this->jointMatrix, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->jointTransformMatrix, dx::XMMatrixIdentity());

	dx::XMStoreFloat4x4(&this->toModelSpace, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->toJointSpace, dx::XMMatrixIdentity());
}

void Joint::Update() {
	// Rotate specific joints.
	if (
		std::string(this->name).compare("thigh.L") != 0 &&
		std::string(this->name).compare("head") != 0 &&
		std::string(this->name).compare("forearm.R") != 0
	) {
		return;
	}

	// Check if quaternion rotation axis is equal to zero.
	if (dx::XMVector4Equal(dx::XMLoadFloat4(&this->rotationQ), dx::XMVectorZero())) {
		this->rotationQ.y = 1.0f;
	}

	// Rotate joint.
	this->rotationQ.w += 0.01f;
	this->rotationQ.w = (float) fmod(
		(double) this->rotationQ.w,
		(double) dx::XM_PI*2.0f
	);

	// Build transformation matrix that will be applied to vertex at joint space.
	dx::XMStoreFloat4x4(
		&this->jointTransformMatrix,
		dx::XMMatrixRotationQuaternion(
			dx::XMQuaternionRotationAxis(
				dx::XMLoadFloat3(
					&dx::XMFLOAT3(
						this->rotationQ.x, this->rotationQ.y, this->rotationQ.z
					)
				),
				this->rotationQ.w
			)
		)
	);

	// Update final transformation matrix that will be applied to each vertex this joint influences.
	dx::XMStoreFloat4x4(
		&this->jointMatrix,
		dx::XMMatrixTranspose(
			// First transform vertex (which is in model space) to joint space.
			dx::XMLoadFloat4x4(&this->toJointSpace) *
			// Transform vertex relative to joint's position (0, 0, 0).
			dx::XMLoadFloat4x4(&this->jointTransformMatrix) *
			// Finally transform vertex back to model space.
			dx::XMLoadFloat4x4(&this->toModelSpace)
		)
	);

	this->dataChanged = true;
}