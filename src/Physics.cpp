#include "Physics.h"

Physics::Physics(Vector3 gravity, float stepPerSecond) {

	// Set class values.
	this->stepPerSecond = stepPerSecond;

	// Create foundation for PhysX.
	this->pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, this->pxAllocator, this->pxErrorCallback);
	if (this->pxFoundation == NULL) { OutputDebugStringA("PxCreateFoundation error"); }

	// Create Visual Debugger.
	this->pxPvd = PxCreatePvd(*(this->pxFoundation));
	if (this->pxPvd == NULL) {
		OutputDebugStringA("PxCreatePvd error");
	}
	PxPvdTransport* pxTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	pxPvd->connect(*pxTransport, PxPvdInstrumentationFlag::eALL);

	// Create physics object.
	this->pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, (*pxFoundation), PxTolerancesScale(), false, this->pxPvd);
	if (this->pxPhysics == NULL){OutputDebugStringA("PxCreatePhysics error");}

	// CPU dispatcher.
	PxDefaultCpuDispatcher* pxDispatcher = PxDefaultCpuDispatcherCreate(2, NULL);

	// Scene.
	PxSceneDesc pxSceneDesc = PxSceneDesc(this->pxPhysics->getTolerancesScale());
	pxSceneDesc.gravity = PxVec3(gravity.x, gravity.y, gravity.z);
	pxSceneDesc.cpuDispatcher = pxDispatcher;
	pxSceneDesc.filterShader = PxDefaultSimulationFilterShader;

	this->pxScene = this->pxPhysics->createScene(pxSceneDesc);
	if (this->pxScene == NULL) { OutputDebugStringA("createScene error"); }

	// Debugger client.
	PxPvdSceneClient* pvdClient = this->pxScene->getScenePvdClient();
	if(pvdClient){
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
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
	if (pEntity->pCollisionShape == NULL) {
		return false;
	}

	this->pxScene->addActor(*(pEntity->pActor));
	return true;
}