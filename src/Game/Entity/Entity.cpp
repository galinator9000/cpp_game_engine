#include "Entity.h"
#include "Mesh/Mesh.h"

// Default constructor.
Entity::Entity(){
	this->gSize = Vector3(1, 1, 1);
	this->gPosition = Vector3(0, 0, 0);
	this->gRotationQ = Vector4(1, 0, 0, 0);

	this->gMaterial = new Material(NULL);
	this->collisionMaterial = new CollisionMaterial();

	this->updateConstantBuffer();
}

Entity::Entity(
	Vector3 size, Vector3 position, Vector4 rotationQ,
	Material* material, CollisionMaterial* collisionMaterial,
	Mesh* pMesh,
	CollisionShape* pCollisionShape,
	CollisionActor* pCollisionActor
){
	this->gSize = size;
	this->gPosition = position;

	if (Vector3(rotationQ.x, rotationQ.y, rotationQ.z) == Vector3(0,0,0)) {
		rotationQ.x = 1;
	}
	this->gRotationQ = rotationQ;

	if (material == NULL) {
		material = new Material(NULL);
	}
	this->gMaterial = material;
	if (collisionMaterial == NULL) {
		collisionMaterial = new CollisionMaterial();
	}
	this->collisionMaterial = collisionMaterial;

	// Attach mesh object if given.
	if (pMesh != NULL) {
		this->attachMesh((Mesh*) pMesh);
	}

	// Attach collision objects if given.
	if (pCollisionShape != NULL) {
		this->attachCollisionShape((CollisionShape*) pCollisionShape);
	}
	if (pCollisionActor != NULL) {
		this->attachCollisionActor((CollisionActor*) pCollisionActor);
	}

	this->updateConstantBuffer();
}

void Entity::Update() {
	if (this->useMeshDeformer) {
		this->meshDeformer->Update();
	}

	this->gMaterial->Update();

	if (!this->dataChanged) {
		this->shouldUpdateGPUData = false;
		return;
	}

	this->updateConstantBuffer();
}

void Entity::updateConstantBuffer() {
	// Update VS constant buffer.
	// Update world matrix.
	dx::XMStoreFloat4x4(
		&(this->gEntityVSConstantBuffer.worldMatrix),
		dx::XMMatrixTranspose(
			// Local
			dx::XMMatrixScalingFromVector(this->gSize.loadXMVECTOR()) *

			dx::XMMatrixRotationQuaternion(
				dx::XMQuaternionRotationAxis(
					this->gRotationQ.loadXMVECTOR(),
					this->gRotationQ.w
				)
			) *

			dx::XMMatrixTranslationFromVector(this->gPosition.loadXMVECTOR())
		)
	);
	this->gEntityVSConstantBuffer.useMeshDeformer = (unsigned int) this->useMeshDeformer;

	// Graphics object will check this if buffer should be updated or not.
	this->shouldUpdateGPUData = true;
}

void Entity::Reset() {
	this->dataChanged = false;
	this->shouldUpdateGPUData = false;
}

// Mesh
bool Entity::attachMesh(Mesh* mesh) {
	if (this->mesh == NULL) {
		this->mesh = mesh;
		this->isDrawable = true;
		return true;
	}
	return false;
}

// Mesh deformer
bool Entity::attachMeshDeformer(MeshDeformer* meshDeformer) {
	if (this->meshDeformer == NULL) {
		this->meshDeformer = meshDeformer;
		this->useMeshDeformer = true;
		this->gEntityVSConstantBuffer.useMeshDeformer = this->useMeshDeformer;

		// Give Skeleton information to MeshDeformer class from current Mesh.
		this->meshDeformer->Setup(
			&this->mesh->gSkeleton,
			this->gSize
		);

		return true;
	}
	return false;
}

bool Entity::setAnimation(const char* animName) {
	if (!this->useMeshDeformer || this->meshDeformer == NULL) {
		return false;
	}

	Animation* animation = this->mesh->getAnimation(animName);
	if (animation == NULL) {
		return false;
	}

	this->meshDeformer->setAnimation(animation);

	return true;
}

// Hierarchy system
void Entity::attachChild(Entity* child){
	this->childEntities.push_back(child);
	child->parentEntity = this;

	this->dataChanged = true;
	child->dataChanged = true;
}

void Entity::detachChild(Entity* child){
	this->childEntities.erase(
		std::remove(this->childEntities.begin(), this->childEntities.end(), child),
		this->childEntities.end()
	);
	child->parentEntity = NULL;

	this->dataChanged = true;
	child->dataChanged = true;
}

Entity* Entity::getChild(int childIndex) {
	return this->childEntities.at(childIndex);
}

unsigned int Entity::getChildCount() {
	return (unsigned int) this->childEntities.size();
}

// General
// Position & Rotation & Scaling setters.
void Entity::setPosition(Vector3 position) {
	this->gPosition = position;

	// Update physics actor.
	if (this->pCollisionActor != NULL) {
		this->pCollisionActor->setPosition(position);
	}

	this->dataChanged = true;
}

void Entity::setRotation(Vector4 rotationQ) {
	this->gRotationQ = rotationQ;

	// Update physics actor.
	if (this->pCollisionActor != NULL) {
		this->pCollisionActor->setRotation(rotationQ);
	}

	this->dataChanged = true;
}

void Entity::setScale(Vector3 scaling) {
	this->gSize = scaling;
	this->dataChanged = true;
}

void Entity::Translate(Vector3 translationVector){
	this->gPosition = this->gPosition + translationVector;
	this->dataChanged = true;
}

void Entity::rotateQuaternion(Vector4 rotationQuaternionVector){
	this->gRotationQ = rotationQuaternionVector;
	this->dataChanged = true;
}

void Entity::Scale(Vector3 scalingVector){
	this->gSize = this->gSize * scalingVector;
	this->dataChanged = true;
}

//// Physics
void Entity::attachCollisionShape(CollisionShape* collisionShape) {
	this->pCollisionShape = collisionShape;
}

void Entity::attachCollisionActor(CollisionActor* collisionActor) {
	this->pCollisionActor = collisionActor;
}