#include "Physics.h"

Physics::Physics(Vector3 gravity, float stepPerSecond) {

	// Set class values.
	this->stepPerSecond = stepPerSecond;

	// Create foundation for PhysX.
	this->pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, this->pxAllocator, this->pErrorCallback);
	if (this->pxFoundation == NULL) {
		OutputDebugStringA("PxCreateFoundation error");
	}

	// Create Visual Debugger.
	this->pxPvd = PxCreatePvd(*(this->pxFoundation));
	if (this->pxPvd == NULL) {
		OutputDebugStringA("PxCreatePvd error");
	}
	PxPvdTransport* pxTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	this->pxPvd->connect(*pxTransport, PxPvdInstrumentationFlag::eALL);

	// Create physics object.
	PxTolerancesScale pxTolerancesScale = PxTolerancesScale();
	this->pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, (*this->pxFoundation), pxTolerancesScale, false, this->pxPvd);
	if (this->pxPhysics == NULL){
		OutputDebugStringA("PxCreatePhysics error");
	}

	// Initialize extensions.
	PxInitExtensions(*(this->pxPhysics), this->pxPvd);

	// CPU dispatcher.
	PxDefaultCpuDispatcher* pxDispatcher = PxDefaultCpuDispatcherCreate(2, NULL);

	// Scene.
	PxSceneDesc pxSceneDesc = PxSceneDesc(this->pxPhysics->getTolerancesScale());
	pxSceneDesc.gravity = PxVec3(gravity.x, gravity.y, gravity.z);
	pxSceneDesc.cpuDispatcher = pxDispatcher;
	pxSceneDesc.filterShader = PxDefaultSimulationFilterShader;

	this->pxScene = this->pxPhysics->createScene(pxSceneDesc);
	if (this->pxScene == NULL){ 
		OutputDebugStringA("createScene error");
	}
	this->pxScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	this->pxScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);

	// Debugger client.
	PxPvdSceneClient* pvdClient = this->pxScene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	// Controller Manager for CCT (Character Controller)
	this->pxControllerManager = PxCreateControllerManager(*(this->pxScene));
}

Physics::~Physics(){
	this->pxPhysics->release();
	this->pxFoundation->release();
}

void Physics::Update(){
	this->pxScene->simulate(1.0f / this->stepPerSecond);
	this->pxScene->fetchResults(true);
}

bool Physics::addEntity(Entity* pEntity){
	// Check if entity have any physical representation.
	if (
		pEntity->pCollisionActor == NULL ||
		pEntity->pCollisionShape == NULL
	) {
		return false;
	}

	// Create physical material
	pEntity->pCollisionShape->pMaterial = this->pxPhysics->createMaterial(
		pEntity->collisionMaterial->staticFriction,
		pEntity->collisionMaterial->dynamicFriction,
		pEntity->collisionMaterial->restitution
	);

	// CCT (Character Controller)
	if (pEntity->pCollisionActor->actorType == COLLISION_ACTOR_CCT) {
		// Calculate bounding box that fits to mesh.
		Vector3 meshBBox = pEntity->mesh->calculateBoundingBox();
		meshBBox = meshBBox * pEntity->gSize;
		
		// Capsule
		PxCapsuleControllerDesc pxCDesc;
		pxCDesc.climbingMode = PxCapsuleClimbingMode::eEASY;
		pxCDesc.height = meshBBox.y * 0.8f;
		pxCDesc.radius = meshBBox.y * 0.2f;

		// Box
		/*PxBoxControllerDesc pxCDesc;
		pxCDesc.halfSideExtent = meshScale.x / 2;
		pxCDesc.halfHeight = meshScale.y / 2;
		pxCDesc.halfForwardExtent = meshScale.z / 2;*/

		// General
		pxCDesc.position = PxExtendedVec3(
			pEntity->gPosition.x,
			pEntity->gPosition.y,
			pEntity->gPosition.z
		);
		pxCDesc.upDirection = PxVec3(0, 1, 0);
		pxCDesc.density = pEntity->collisionMaterial->density;
		pxCDesc.material = pEntity->pCollisionShape->pMaterial;
		pxCDesc.contactOffset = 0.1f;

		pEntity->pCollisionActor->pCharacterController = this->pxControllerManager->createController(pxCDesc);
		pEntity->pCollisionActor->pCharacterController->setFootPosition(
			PxExtendedVec3(
				(float) pEntity->gPosition.x,
				(float) pEntity->gPosition.y,
				(float) pEntity->gPosition.z
			)
		);

		this->setupEntityRagdoll(pEntity);
		return true;
	}

	// Entity needs geometry for creation, if not provided, create a bounding box for it.
	if (pEntity->pCollisionShape->pGeometry == NULL) {
		Vector3 meshBBox = pEntity->mesh->calculateBoundingBox();
		meshBBox = meshBBox * pEntity->gSize;
		pEntity->pCollisionShape->createBoxGeometry(meshBBox / 2);
	}

	// Create shape
	pEntity->pCollisionShape->pShape = this->pxPhysics->createShape(
		*(pEntity->pCollisionShape->pGeometry),
		*(pEntity->pCollisionShape->pMaterial)
	);

	// Initial transform
	PxTransform rigidActorTransform = PxTransform(
		PxVec3(pEntity->gPosition.x, pEntity->gPosition.y, pEntity->gPosition.z),
		PxQuat(pEntity->gRotationQ.x, pEntity->gRotationQ.y, pEntity->gRotationQ.z, pEntity->gRotationQ.w)
	);

	// Physical actor of the entity
	PxActor* rigidActor = NULL;

	// Static actor
	if (pEntity->pCollisionActor->actorType == COLLISION_ACTOR_STATIC) {
		PxRigidStatic* rigidStaticActor = this->pxPhysics->createRigidStatic(rigidActorTransform);

		rigidStaticActor->attachShape(*(pEntity->pCollisionShape->pShape));

		rigidActor = rigidStaticActor;
	}
	// Dynamic actor
	if (pEntity->pCollisionActor->actorType == COLLISION_ACTOR_DYNAMIC) {
		PxRigidDynamic* rigidDynamicActor = this->pxPhysics->createRigidDynamic(rigidActorTransform);

		rigidDynamicActor->attachShape(*(pEntity->pCollisionShape->pShape));
		PxRigidBodyExt::updateMassAndInertia(*rigidDynamicActor, pEntity->collisionMaterial->density);

		rigidActor = rigidDynamicActor;
	}
	// Kinematic actor
	if (pEntity->pCollisionActor->actorType == COLLISION_ACTOR_KINEMATIC) {
		PxRigidDynamic* rigidDynamicActor = this->pxPhysics->createRigidDynamic(rigidActorTransform);

		rigidDynamicActor->attachShape(*(pEntity->pCollisionShape->pShape));
		rigidDynamicActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		
		rigidActor = rigidDynamicActor;
	}

	if (rigidActor == NULL) {
		return false;
	}

	// Update rigid actor pointer.
	pEntity->pCollisionActor->pActor = rigidActor;

	// Add actor to scene.
	this->pxScene->addActor(*(pEntity->pCollisionActor->pActor));

	return true;
}

void Physics::setupEntityRagdoll(Entity* pEntity) {
	// Construct ragdoll actors if used.
	if (pEntity->useMeshDeformer) {
		MeshDeformer* pMeshDeformer = pEntity->meshDeformer;

		// Create aggregate object for clustering joints of the deformer.
		pEntity->meshDeformer->ragdollAggregate = this->pxPhysics->createAggregate(pEntity->meshDeformer->gJointCount, true);
		this->pxScene->addAggregate(*(pMeshDeformer->ragdollAggregate));

		for (unsigned int j = 0; j < pEntity->meshDeformer->gJointCount; j++) {
			// Create material and shape of the joint.
			pMeshDeformer->pRagdollCollisionShape[j]->pMaterial = this->pxPhysics->createMaterial(
				pEntity->collisionMaterial->staticFriction,
				pEntity->collisionMaterial->dynamicFriction,
				pEntity->collisionMaterial->restitution
			);
			pMeshDeformer->pRagdollCollisionShape[j]->pShape = this->pxPhysics->createShape(
				*(pMeshDeformer->pRagdollCollisionShape[j]->pGeometry),
				*(pMeshDeformer->pRagdollCollisionShape[j]->pMaterial)
			);

			// Create physical actor
			// Translate joint actors to CCT's foot position.
			pMeshDeformer->pRagdollCollisionActor[j]->initialTransform = PxTransform(
				(pEntity->pCollisionActor->pCharacterController->getUpDirection() *
				pEntity->pCollisionActor->pCharacterController->getContactOffset()) + 
				PxVec3(10, 0, 0)
			) * pMeshDeformer->pRagdollCollisionActor[j]->initialTransform;

			PxRigidDynamic* rigidDynamicActor = this->pxPhysics->createRigidDynamic(
				pMeshDeformer->pRagdollCollisionActor[j]->initialTransform
			);
			rigidDynamicActor->attachShape(*(pMeshDeformer->pRagdollCollisionShape[j]->pShape));
			PxRigidBodyExt::updateMassAndInertia(*rigidDynamicActor, 10.0f);
			pMeshDeformer->pRagdollCollisionActor[j]->pActor = rigidDynamicActor;

			// Static actor for debugging.
			/*PxRigidStatic* rigidStaticActor = this->pxPhysics->createRigidStatic(pMeshDeformer->pRagdollCollisionActor[j]->initialTransform);
			rigidStaticActor->attachShape(*(pMeshDeformer->pRagdollCollisionShape[j]->pShape));
			pMeshDeformer->pRagdollCollisionActor[j]->pActor = rigidStaticActor;*/

			pMeshDeformer->ragdollAggregate->addActor(*(pMeshDeformer->pRagdollCollisionActor[j]->pActor));
			rigidDynamicActor->putToSleep();
		}

		for (unsigned int j = 0; j < pEntity->meshDeformer->gJointCount; j++) {
			// Create physical joint.
			// Skip root joint, physical joints are constructed relative to parent.
			if (pMeshDeformer->skeleton->gJoints[j]->isRoot) {
				continue;
			}

			this->createSphericalJointRagdoll(
				pMeshDeformer->pRagdollCollisionActor[j],
				pMeshDeformer->pRagdollCollisionActor[j]->parentActor,
				pMeshDeformer->pRagdollCollisionActor[j]->jointPointSelf,
				pMeshDeformer->pRagdollCollisionActor[j]->jointPointParent
			);
		}
	}
}

void Physics::updateEntity(Entity* pEntity) {
	// Check if entity have any physical representation.
	if (pEntity->pCollisionActor == NULL) {
		return;
	}

	// If entity is static, we can simply return.
	if (pEntity->pCollisionActor->actorType == COLLISION_ACTOR_STATIC) {
		return;
	}

	// Initial transform
	PxTransform tm = PxTransform(
		pEntity->gPosition.toPxVec3(),
		pEntity->gRotationQ.toPxQuat()
	);

	// Dynamic
	if (pEntity->pCollisionActor->actorType == COLLISION_ACTOR_DYNAMIC){
		// Get dynamic rigid object.
		PxRigidDynamic* rigidDynamic = pEntity->pCollisionActor->pActor->is<PxRigidDynamic>();
		if (rigidDynamic == NULL) {
			return;
		}
		// Check if it is in sleeping state.
		if (rigidDynamic->isSleeping()) {
			return;
		}

		// Get global transform of the actor.
		tm = rigidDynamic->getGlobalPose();
	}
	// Kinematic
	if (pEntity->pCollisionActor->actorType == COLLISION_ACTOR_KINEMATIC) {
		// Get rigid object.
		PxRigidDynamic* rigidDynamic = pEntity->pCollisionActor->pActor->is<PxRigidDynamic>();
		if (rigidDynamic == NULL) {
			return;
		}

		// Get global transform of the actor.
		tm = rigidDynamic->getGlobalPose();
	}
	// CCT (Character Controller)
	if (pEntity->pCollisionActor->actorType == COLLISION_ACTOR_CCT) {
		// Get position of the CCT
		PxExtendedVec3 tmExt = pEntity->pCollisionActor->pCharacterController->getFootPosition();

		tm = PxTransform(
			PxVec3(
				(float) tmExt.x,
				(float) tmExt.y,
				(float) tmExt.z
			)
		);
	}

	// Integrate entities' physics position and rotation with graphics side.
	pEntity->gPosition.x = tm.p.x;
	pEntity->gPosition.y = tm.p.y;
	pEntity->gPosition.z = tm.p.z;

	// Get quaternion vector as radians.
	PxVec3 axisVector;
	tm.q.toRadiansAndUnitAxis(pEntity->gRotationQ.w, axisVector);
	pEntity->gRotationQ.x = axisVector.x;
	pEntity->gRotationQ.y = axisVector.y;
	pEntity->gRotationQ.z = axisVector.z;
	pEntity->dataChanged = true;
}

bool Physics::createFixedJoint(CollisionActor* collisionActor1, CollisionActor* collisionActor2, PxTransform transform1, PxTransform transform2){
	if (collisionActor1 == NULL || collisionActor2 == NULL) {
		return false;
	}
	PxRigidActor* pCollisionActor1 = collisionActor1->pActor->is<PxRigidActor>();
	PxRigidActor* pCollisionActor2 = collisionActor2->pActor->is<PxRigidActor>();
	if (pCollisionActor1 == NULL || pCollisionActor2 == NULL) {
		return false;
	}

	PxFixedJoint* pJoint = PxFixedJointCreate(
		*(this->pxPhysics),
		pCollisionActor1,
		transform1,
		pCollisionActor2,
		transform2
	);
	if (pJoint == NULL) {
		return false;
	}
	
	pJoint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	return true;
}

bool Physics::createDistanceJoint(CollisionActor* collisionActor1, CollisionActor* collisionActor2, PxTransform transform1, PxTransform transform2){
	if (collisionActor1 == NULL || collisionActor2 == NULL) {
		return false;
	}
	PxRigidActor* pCollisionActor1 = collisionActor1->pActor->is<PxRigidActor>();
	PxRigidActor* pCollisionActor2 = collisionActor2->pActor->is<PxRigidActor>();
	if (pCollisionActor1 == NULL || pCollisionActor2 == NULL) {
		return false;
	}

	PxDistanceJoint* pJoint = PxDistanceJointCreate(
		*(this->pxPhysics),
		pCollisionActor1,
		transform1,
		pCollisionActor2,
		transform2
	);
	if (pJoint == NULL) {
		return false;
	}

	pJoint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	return true;
}

bool Physics::createSphericalJoint(CollisionActor* collisionActor1, CollisionActor* collisionActor2, PxTransform transform1, PxTransform transform2){
	if (collisionActor1 == NULL || collisionActor2 == NULL) {
		return false;
	}
	PxRigidActor* pCollisionActor1 = collisionActor1->pActor->is<PxRigidActor>();
	PxRigidActor* pCollisionActor2 = collisionActor2->pActor->is<PxRigidActor>();
	if (pCollisionActor1 == NULL || pCollisionActor2 == NULL) {
		return false;
	}

	PxSphericalJoint* pJoint = PxSphericalJointCreate(
		*(this->pxPhysics),
		pCollisionActor1,
		transform1,
		pCollisionActor2,
		transform2
	);
	if (pJoint == NULL) {
		return false;
	}

	pJoint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	return true;
}

bool Physics::createRevoluteJoint(CollisionActor* collisionActor1, CollisionActor* collisionActor2, PxTransform transform1, PxTransform transform2){
	if (collisionActor1 == NULL || collisionActor2 == NULL) {
		return false;
	}
	PxRigidActor* pCollisionActor1 = collisionActor1->pActor->is<PxRigidActor>();
	PxRigidActor* pCollisionActor2 = collisionActor2->pActor->is<PxRigidActor>();
	if (pCollisionActor1 == NULL || pCollisionActor2 == NULL) {
		return false;
	}

	PxRevoluteJoint* pJoint = PxRevoluteJointCreate(
		*(this->pxPhysics),
		pCollisionActor1,
		transform1,
		pCollisionActor2,
		transform2
	);
	if (pJoint == NULL) {
		return false;
	}

	pJoint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	return true;
}

bool Physics::createSphericalJointRagdoll(CollisionActor* collisionActor1, CollisionActor* collisionActor2, PxTransform transform1, PxTransform transform2) {
	if (collisionActor1 == NULL || collisionActor2 == NULL) {
		return false;
	}
	PxRigidActor* pCollisionActor1 = collisionActor1->pActor->is<PxRigidActor>();
	PxRigidActor* pCollisionActor2 = collisionActor2->pActor->is<PxRigidActor>();
	if (pCollisionActor1 == NULL || pCollisionActor2 == NULL) {
		return false;
	}

	PxSphericalJoint* pJoint = PxSphericalJointCreate(
		*(this->pxPhysics),
		pCollisionActor1,
		transform1,
		pCollisionActor2,
		transform2
	);
	if (pJoint == NULL) {
		return false;
	}

	// 45-degree limit on Y and Z axes.
	pJoint->setLimitCone(PxJointLimitCone(PxPi / 4, PxPi / 4, 0.05f));
	pJoint->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);

	pJoint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	return true;
}