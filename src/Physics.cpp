#include "Physics.h"

Physics::Physics(float Gravity, float stepPerSecond){
	// Set class values.
	this->stepPerSecond = stepPerSecond;

	// Create world.
	btCollisionConfiguration* ColCfg = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* ColDsp = new btCollisionDispatcher(ColCfg);
	btBroadphaseInterface* BrdPhs = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* CnstSlv = new btSequentialImpulseConstraintSolver;
	World = new btDiscreteDynamicsWorld(
		ColDsp,
		BrdPhs,
		CnstSlv,
		ColCfg
	);
	World->setGravity(btVector3(0, Gravity, 0));

	// Add rigid body.
	btCollisionShape* box_shape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
	btRigidBody* body = new btRigidBody(btScalar(1.0), new btDefaultMotionState, box_shape);
	World->addRigidBody(body);
}

void Physics::Update(){
	this->World->stepSimulation(1.0f / this->stepPerSecond, 10);
}