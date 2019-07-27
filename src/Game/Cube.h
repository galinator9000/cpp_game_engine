#pragma once
#include "BaseEntity.h"

class Cube : public BaseEntity {
public:
	Cube(btVector3 position, btQuaternion rotation, btVector3 size, float mass);
};