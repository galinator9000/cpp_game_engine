#pragma once
#include "PxPhysicsAPI.h"
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <string>

#include "Entity/Entity.h"
#include "Entity/Shapes.h"

using namespace physx;

class Physics{
	class PhysicsErrorCallback : public PxErrorCallback {
	public:
		void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) {
			std::ostringstream myStream;
			myStream << std::string(message);

			OutputDebugStringA(myStream.str().c_str());
			std::cout << myStream.str().c_str();
		}
	};

public:
	Physics(Vector3 gravity, float stepPerSecond);
	~Physics();

	// Entity
	void Update();
	bool addEntity(Entity* bEntity);
	void setupEntityRagdoll(Entity* pEntity);
	void updateEntity(Entity* bEntity);

	// Physical Joints
	std::vector<PxJoint*> pJoints;
	bool createFixedJoint(
		CollisionActor* collisionActor1, CollisionActor* collisionActor2,
		Vector3& transform1, Vector3& transform2
	);
	bool createDistanceJoint(
		CollisionActor* collisionActor1, CollisionActor* collisionActor2,
		Vector3& transform1, Vector3& transform2
	);
	bool createSphericalJoint(
		CollisionActor* collisionActor1, CollisionActor* collisionActor2,
		Vector3& transform1, Vector3& transform2
	);
	bool createRevoluteJoint(
		CollisionActor* collisionActor1, CollisionActor* collisionActor2,
		Vector3& transform1, Vector3& transform2
	);

public:
	// PhysX main object pointers
	PxPhysics* pxPhysics = NULL;
	PxScene* pxScene = NULL;
	PxControllerManager* pxControllerManager = NULL;
private:
	PxFoundation* pxFoundation = NULL;
	PxPvd* pxPvd;

	// Default objects
	PxDefaultAllocator pxAllocator;
	PhysicsErrorCallback pErrorCallback;

	float stepPerSecond;
};