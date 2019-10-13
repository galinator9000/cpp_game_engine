#include "CollisionActor.h"

CollisionActor::CollisionActor(COLLISION_ACTOR_TYPE actorType) {
	this->actorType = actorType;
}

void CollisionActor::setPosition(Vector3 position) {
	if (
		this->actorType == COLLISION_ACTOR_DYNAMIC ||
		this->actorType == COLLISION_ACTOR_KINEMATIC
	) {
		PxRigidDynamic* rigidDynamic = this->pActor->is<PxRigidDynamic>();

		rigidDynamic->setGlobalPose(
			PxTransform(
				position.toPxVec3(),
				rigidDynamic->getGlobalPose().q
			)
		);
	}
}

void CollisionActor::setRotation(Vector4 rotationQ) {
	if (
		this->actorType == COLLISION_ACTOR_DYNAMIC ||
		this->actorType == COLLISION_ACTOR_KINEMATIC
		) {
		PxRigidDynamic* rigidDynamic = this->pActor->is<PxRigidDynamic>();

		rigidDynamic->setGlobalPose(
			PxTransform(
				rigidDynamic->getGlobalPose().p,
				rotationQ.toPxQuat()
			)
		);
	}
}