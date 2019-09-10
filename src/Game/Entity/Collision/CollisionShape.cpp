#include "CollisionShape.h"

CollisionShape::CollisionShape() {

}

void CollisionShape::createBoxGeometry(Vector3 pSize) {
	this->pGeometry = new PxBoxGeometry(
		PxVec3(
			pSize.x,
			pSize.y,
			pSize.z
		)
	);
}