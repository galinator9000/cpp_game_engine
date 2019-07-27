#include "Physics.h"

Physics::Physics(float Gravity, float stepPerSecond){
	// Set class values.
	this->stepPerSecond = stepPerSecond;

	// Create world.
	btCollisionConfiguration* ColCfg = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* ColDsp = new btCollisionDispatcher(ColCfg);
	btBroadphaseInterface* BrdPhs = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* CnstSlv = new btSequentialImpulseConstraintSolver;
	this->phyWorld = new btDiscreteDynamicsWorld(
		ColDsp,
		BrdPhs,
		CnstSlv,
		ColCfg
	);
	this->phyWorld->setGravity(btVector3(0, Gravity, 0));
}

void Physics::Update(){
	this->phyWorld->stepSimulation(1.0f / this->stepPerSecond, 10);
	for (int ico = 0; ico < this->phyWorld->getNumCollisionObjects(); ico++) {
		btCollisionObject* co = this->phyWorld->getCollisionObjectArray()[ico];
		btTransform tsf = co->getWorldTransform();

		if (ico > 0) {
			std::ostringstream myStream;
			myStream << "world pos object " << ico << ", ";
			myStream << float(tsf.getOrigin().getX()) << ", ";
			myStream << float(tsf.getOrigin().getY()) << ", ";
			myStream << float(tsf.getOrigin().getZ()) << "\n";
			OutputDebugStringA(myStream.str().c_str());
		}
	}
}

void Physics::AddEntity(BaseEntity& pBaseEntity){
	this->phyWorld->addRigidBody(pBaseEntity.rigidBody);
	this->collisionShapes.push_back(pBaseEntity.colShape);
}