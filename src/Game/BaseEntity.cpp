#include "BaseEntity.h"
#include "Shapes.h"

PxPhysics* BaseEntity::ppxPhysics;

BaseEntity::~BaseEntity() {
	delete[] this->gVertices;
	delete[] this->gIndices;
}

void BaseEntity::Update(bool initial){}
void BaseEntity::updateConstantBuffer(){}
void BaseEntity::gCreateVerticesAndIndices(){}