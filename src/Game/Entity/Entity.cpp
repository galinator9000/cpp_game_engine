#include "Entity.h"
#include "Mesh/Mesh.h"

// Default constructor.
Entity::Entity(){
	EntityProperties entProp;

	this->gSize = entProp.size;
	this->gPosition = entProp.position;
	this->gRotationQ = entProp.rotationQ;

	this->material.color = entProp.material.color;
	this->material.specularHighlightColor = entProp.material.specularHighlightColor;
	this->material.specularPower = entProp.material.specularPower;
	this->material.specularIntensity = entProp.material.specularIntensity;

	// Physics material information
	this->collisionMaterial.staticFriction = entProp.collisionMaterial.staticFriction;
	this->collisionMaterial.dynamicFriction = entProp.collisionMaterial.dynamicFriction;
	this->collisionMaterial.restitution = entProp.collisionMaterial.restitution;
	this->collisionMaterial.density = entProp.collisionMaterial.density;

	this->updateConstantBuffer();
}

Entity::Entity(EntityProperties entProp){
	this->gSize = entProp.size;
	this->gPosition = entProp.position;
	this->gRotationQ = entProp.rotationQ;

	this->material.color = entProp.material.color;
	this->material.specularHighlightColor = entProp.material.specularHighlightColor;
	this->material.specularPower = entProp.material.specularPower;
	this->material.specularIntensity = entProp.material.specularIntensity;

	// Physics material information
	this->collisionMaterial.staticFriction = entProp.collisionMaterial.staticFriction;
	this->collisionMaterial.dynamicFriction = entProp.collisionMaterial.dynamicFriction;
	this->collisionMaterial.restitution = entProp.collisionMaterial.restitution;
	this->collisionMaterial.density = entProp.collisionMaterial.density;

	// Attach mesh object if given.
	if (entProp.pMesh != NULL) {
		this->attachMesh((Mesh*) entProp.pMesh);
	}

	// Attach collision objects if given.
	if (entProp.pCollisionShape != NULL) {
		this->attachCollisionShape((CollisionShape*) entProp.pCollisionShape);
	}
	if (entProp.pCollisionActor != NULL) {
		this->attachCollisionActor((CollisionActor*) entProp.pCollisionActor);
	}

	this->updateConstantBuffer();
}

void Entity::Update() {
	if (this->useMeshDeformer) {
		this->meshDeformer->Update();
	}

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

	//// Update PS constant buffer.
	// Material information.
	this->gEntityPSConstantBuffer.color = this->material.color.loadXMFLOAT();
	this->gEntityPSConstantBuffer.specularHighlightColor = this->material.specularHighlightColor.loadXMFLOAT();
	this->gEntityPSConstantBuffer.specularPower = this->material.specularPower;
	this->gEntityPSConstantBuffer.specularIntensity = this->material.specularIntensity;

	this->gEntityPSConstantBuffer.useTexture = (unsigned int) this->useTexture;
	if (this->texture != NULL) {
		this->gEntityPSConstantBuffer.useNormalMapping = (unsigned int) this->texture->useNormalMapping;
		this->gEntityPSConstantBuffer.useAlpha = (unsigned int) this->texture->useAlpha;
	}
	else {
		this->gEntityPSConstantBuffer.useNormalMapping = false;
		this->gEntityPSConstantBuffer.useAlpha = false;
	}

	// Graphics object will check this if buffer should be updated or not.
	this->shouldUpdateGPUData = true;
}

void Entity::Reset() {
	this->dataChanged = false;
	this->shouldUpdateGPUData = false;
}

// Texture
void Entity::attachTexture(Texture* texture){
	if (texture == NULL) {
		return;
	}

	this->texture = texture;
	this->useTexture = true;
	this->dataChanged = true;
}

void Entity::attachTextureSampler(TextureSampler* textureSampler) {
	if (textureSampler == NULL) {
		return;
	}

	this->textureSampler = textureSampler;
	this->dataChanged = true;
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
void Entity::setColor(Color color) {
	this->material.color = color;
	this->dataChanged = true;
}

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