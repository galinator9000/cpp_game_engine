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
	pxPvd->connect(*pxTransport, PxPvdInstrumentationFlag::eALL);

	// Create physics object.
	this->pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, (*this->pxFoundation), PxTolerancesScale(), false, this->pxPvd);
	if (this->pxPhysics == NULL){
		OutputDebugStringA("PxCreatePhysics error");
	}

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

	// Debugger client.
	PxPvdSceneClient* pvdClient = this->pxScene->getScenePvdClient();
	if(pvdClient){
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
		pEntity->collisionMaterial.staticFriction,
		pEntity->collisionMaterial.dynamicFriction,
		pEntity->collisionMaterial.restitution
	);

	// CCT (Character Controller)
	if (pEntity->pCollisionActor->actorType == COLLISION_ACTOR_CCT) {
		PxCapsuleControllerDesc pxCDesc;

		// Capsule
		pxCDesc.climbingMode = PxCapsuleClimbingMode::eEASY;
		pxCDesc.height = 4.0f;
		pxCDesc.radius = 1.0f;

		// General
		pxCDesc.position = PxExtendedVec3(
			pEntity->gPosition.x,
			pEntity->gPosition.y,
			pEntity->gPosition.z
		);
		pxCDesc.upDirection = PxVec3(0, 1, 0);
		pxCDesc.density = pEntity->collisionMaterial.density;
		pxCDesc.material = pEntity->pCollisionShape->pMaterial;

		pEntity->pCollisionActor->pCharacterController = this->pxControllerManager->createController(pxCDesc);
		pEntity->pCollisionActor->pCharacterController->setFootPosition(
			PxExtendedVec3(
				(float) pEntity->gPosition.x,
				(float) pEntity->gPosition.y,
				(float) pEntity->gPosition.z
			)
		);
		return true;
	}

	//// Entity needs geometry for creation. Check if exists.
	if (pEntity->pCollisionShape->pGeometry == NULL) {
		return false;
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
		PxRigidBodyExt::updateMassAndInertia(*rigidDynamicActor, pEntity->collisionMaterial.density);

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
		PxVec3(pEntity->gPosition.x, pEntity->gPosition.y, pEntity->gPosition.z),
		PxQuat(pEntity->gRotationQ.x, pEntity->gRotationQ.y, pEntity->gRotationQ.z, pEntity->gRotationQ.w)
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