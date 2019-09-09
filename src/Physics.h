#pragma once
#include "PxPhysicsAPI.h"
#include <Windows.h>
#include <sstream>
#include <string>

#include "Entity/Entity.h"
#include "Entity/Shapes.h"

using namespace physx;

class Physics{
public:
	Physics(Vector3 gravity, float stepPerSecond);
	~Physics();

	void Update();
	bool addEntity(Entity* bEntity);

	// PhysX variables
public:
	PxPhysics* pxPhysics;
	PxScene* pxScene;

private:
	float stepPerSecond;

	// Default objects
	PxDefaultAllocator pxAllocator;
	PhysicsErrorCallback pErrorCallback;

	// PhysX main object pointers
	PxFoundation* pxFoundation;
	PxPvd* pxPvd;
};

class PhysicsErrorCallback : public PxErrorCallback {
	void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) {
		std::ostringstream myStream;
		myStream << std::string(message);

		OutputDebugStringA(myStream.str().c_str());
		std::cout << myStream.str().c_str();
	}
};