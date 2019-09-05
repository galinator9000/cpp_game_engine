#include "JointTransform.h"

JointTransform::JointTransform() {
	// Initialize matrices with identity matrix.
	dx::XMStoreFloat4x4(&this->jointModelTransformMatrix, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->jointAnimTransformMatrix, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->jointLocalTransformMatrix, dx::XMMatrixIdentity());
}