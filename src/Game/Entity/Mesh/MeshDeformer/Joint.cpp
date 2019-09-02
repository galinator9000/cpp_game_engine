#include "Joint.h"

Joint::Joint() {
	// Initialize matrices with identity matrix.
	dx::XMStoreFloat4x4(&this->jointMatrix, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->transformMatrix, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->transformLinkMatrix, dx::XMMatrixIdentity());
}