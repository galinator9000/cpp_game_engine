#include "BaseEntity.h"
#include "Shapes.h"

PxPhysics* BaseEntity::ppxPhysics;

void BaseEntity::createVertexAndIndexBuffer() {
	switch (this->type) {
		case ENTITY_TYPE::BOX:
			((Box*) this)->createVertices(); 
			((Box*) this)->createIndices();
			break;
		case ENTITY_TYPE::PLANE:
			break;
	}
}