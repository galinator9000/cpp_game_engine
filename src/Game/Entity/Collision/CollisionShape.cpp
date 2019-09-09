#include "CollisionShape.h"

CollisionShape::CollisionShape(Vector3 pSize) {
	this->pSize = pSize;
}

void CollisionShape::createBoxGeometry() {
	this->pGeometry = &(
		PxBoxGeometry(
			PxVec3(
				this->pSize.x,
				this->pSize.y,
				this->pSize.z
			)
		)
	);
}