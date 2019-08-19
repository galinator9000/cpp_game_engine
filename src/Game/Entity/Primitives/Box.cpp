#include "Box.h"

Box::Box(Vector3 size, Vector3 position, Vector3 rotation, Vector3 material) {
	this->pShape = this->ppxPhysics->createShape(
		PxBoxGeometry(PxVec3(size.x, size.y, size.z)),
		*(this->ppxPhysics->createMaterial(material.x, material.y, material.z))
	);

	PxQuat rotationQuaternion;
	PxTransform transform(PxVec3(position.x, position.y, position.z));
	this->rigidDynamic = this->ppxPhysics->createRigidDynamic(transform);
	this->rigidDynamic->attachShape(*pShape);
	PxRigidBodyExt::updateMassAndInertia(*(this->rigidDynamic), 10.0f);
	this->pActor = this->rigidDynamic;

	// Graphics
	this->gSize = XMFLOAT3(size.x, size.y, size.z);
	this->gPosition = XMFLOAT3(position.x, position.y, position.z);
	//this->gRotationQ = XMFLOAT4(rotationQuaternion.x, rotationQuaternion.y, rotationQuaternion.z, rotationQuaternion.w);
	this->gRotationQ = XMFLOAT4(0, 0, 0, 0);

	// Set properties of the entity.
	this->type = ENTITY_TYPE::BOX;
	this->isDynamic = true;

	this->Update(true);
}

Box::~Box(){}

void Box::Update(bool initial){
	// Skip static and sleeping dynamic entities.
	this->shouldUpdateData = true;

	if(!initial){
		// Static check.
		if(!this->isDynamic){
			this->shouldUpdateData = false;
			return;
		}
		// Dynamic and sleeping check.
		if(this->isDynamic && this->rigidDynamic->isSleeping()){
			this->shouldUpdateData = false;
			return;
		}
	}

	// Integrate entities' physics position and rotation with graphics side.
	PxTransform tm = this->rigidDynamic->getGlobalPose();
	this->gPosition.x = tm.p.x;
	this->gPosition.y = tm.p.y;
	this->gPosition.z = tm.p.z;
	this->gRotationQ.x = tm.q.x;
	this->gRotationQ.y = tm.q.y;
	this->gRotationQ.z = tm.q.z;
	this->gRotationQ.w = tm.q.w;	

	this->updateConstantBuffer();
}

void Box::updateConstantBuffer() {
	// Update constant buffer that held on class.
	dx::XMStoreFloat4x4(
		&(this->gConstBuffer.worldMatrix),
		dx::XMMatrixTranspose(
			dx::XMMatrixScaling(this->gSize.x, this->gSize.y, this->gSize.z) *
			dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&this->gRotationQ)) *
			dx::XMMatrixTranslation(this->gPosition.x, this->gPosition.y, this->gPosition.z)
		)
	);
}

void Box::gCreateVerticesAndIndices() {
	// 3D Cube vertices
	Vertex _vertices[] = {
		// Front (Normal -Z)
		{{ -1.0f, -1.0f, -1.0f }, {0, 0, -1}},
		{{ 1.0f, -1.0f, -1.0f }, {0, 0, -1}},
		{{ -1.0f, 1.0f, -1.0f }, {0, 0, -1}},
		{{ 1.0f, 1.0f, -1.0f }, {0, 0, -1}},

		// Back (Normal +Z)
		{{ 1.0f, -1.0f, 1.0f }, {0, 0, 1}},
		{{ -1.0f, -1.0f, 1.0f }, {0, 0, 1}},
		{{ 1.0f, 1.0f, 1.0f }, {0, 0, 1}},
		{{ -1.0f, 1.0f, 1.0f }, {0, 0, 1}},

		// Right (Normal +X)
		{{ 1.0f, -1.0f, -1.0f }, {1, 0, 0}},
		{{ 1.0f, -1.0f, 1.0f }, {1, 0, 0}},
		{{ 1.0f, 1.0f, -1.0f }, {1, 0, 0}},
		{{ 1.0f, 1.0f, 1.0f }, {1, 0, 0}},

		// Left (Normal -X)
		{{ -1.0f, -1.0f, 1.0f }, {-1, 0, 0}},
		{{ -1.0f, -1.0f, -1.0f }, {-1, 0, 0}},
		{{ -1.0f, 1.0f, 1.0f }, {-1, 0, 0}},
		{{ -1.0f, 1.0f, -1.0f }, {-1, 0, 0}},

		// Top (Normal +Y)
		{{ -1.0f, 1.0f, -1.0f }, {0, 1, 0}},
		{{ 1.0f, 1.0f, -1.0f }, {0, 1, 0}},
		{{ -1.0f, 1.0f, 1.0f }, {0, 1, 0}},
		{{ 1.0f, 1.0f, 1.0f }, {0, 1, 0}},

		// Bottom (Normal -Y)
		{{ -1.0f, -1.0f, 1.0f }, {0, -1, 0}},
		{{ 1.0f, -1.0f, 1.0f }, {0, -1, 0}},
		{{ -1.0f, -1.0f, -1.0f }, {0, -1, 0}},
		{{ 1.0f, -1.0f, -1.0f }, {0, -1, 0}},
	};
	this->gVertexCount = (UINT) std::size(_vertices);

	Vertex* vertices = new Vertex[this->gVertexCount];
	std::copy(_vertices, _vertices + this->gVertexCount, vertices);
	this->gVertices = vertices;
	
	// 3D Cube indices
	unsigned int _indices[] = {
		// Front
		0,2,1, 2,3,1,

		// Back
		4,6,5, 6,7,5,

		// Right
		8,10,9, 10,11,9,

		// Left
		12,14,13, 14,15,13,

		// Top
		16,18,17, 18,19,17,

		// Bottom
		20,22,21, 22,23,21
	};
	this->gIndexCount = (UINT) std::size(_indices);

	unsigned int* indices = new unsigned int[this->gIndexCount];
	std::copy(_indices, _indices + this->gIndexCount, indices);
	this->gIndices = indices;
}