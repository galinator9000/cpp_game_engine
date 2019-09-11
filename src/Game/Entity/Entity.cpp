#include "Entity.h"
#include "Shapes.h"

// Default constructor.
Entity::Entity(){
	EntityProperties entProp;

	this->gSize = dx::XMFLOAT3(entProp.size.x, entProp.size.y, entProp.size.z);
	this->gPosition = dx::XMFLOAT3(entProp.position.x, entProp.position.y, entProp.position.z);
	this->gRotationQ = dx::XMFLOAT4(entProp.rotationQ.x, entProp.rotationQ.y, entProp.rotationQ.z, entProp.rotationQ.w);

	this->entityMaterial.color = entProp.color;
	this->entityMaterial.specularPower = 3.0f;
	this->entityMaterial.specularIntensity = 0.5f;

	// Physics material information
	this->collisionMaterial.staticFriction = entProp.collisionMaterial.staticFriction;
	this->collisionMaterial.dynamicFriction = entProp.collisionMaterial.dynamicFriction;
	this->collisionMaterial.restitution = entProp.collisionMaterial.restitution;
	this->collisionMaterial.density = entProp.collisionMaterial.density;

	this->updateConstantBuffer();
}

Entity::Entity(EntityProperties entProp){
	this->gSize = dx::XMFLOAT3(entProp.size.x, entProp.size.y, entProp.size.z);
	this->gPosition = dx::XMFLOAT3(entProp.position.x, entProp.position.y, entProp.position.z);
	this->gRotationQ = dx::XMFLOAT4(entProp.rotationQ.x, entProp.rotationQ.y, entProp.rotationQ.z, entProp.rotationQ.w);

	this->entityMaterial.color = entProp.color;
	this->entityMaterial.specularPower = 3.0f;
	this->entityMaterial.specularIntensity = 0.5f;

	// Physics material information
	this->collisionMaterial.staticFriction = entProp.collisionMaterial.staticFriction;
	this->collisionMaterial.dynamicFriction = entProp.collisionMaterial.dynamicFriction;
	this->collisionMaterial.restitution = entProp.collisionMaterial.restitution;
	this->collisionMaterial.density = entProp.collisionMaterial.density;

	// Attach mesh object if given.
	if (entProp.mesh != NULL) {
		this->attachMesh((Mesh*) entProp.mesh);
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
			dx::XMMatrixScalingFromVector(dx::XMLoadFloat3(&this->gSize)) *

			dx::XMMatrixRotationQuaternion(
				dx::XMQuaternionRotationAxis(
					dx::XMLoadFloat3(
						&dx::XMFLOAT3(
							this->gRotationQ.x,
							this->gRotationQ.y,
							this->gRotationQ.z
						)
					),
					this->gRotationQ.w
				)
			) *

			dx::XMMatrixTranslationFromVector(dx::XMLoadFloat3(&this->gPosition))
		)
	);

	//// Update PS constant buffer.
	// Material information.
	this->gEntityPSConstantBuffer.color = dx::XMFLOAT3(
		this->entityMaterial.color.r,
		this->entityMaterial.color.g,
		this->entityMaterial.color.b
	);
	this->gEntityPSConstantBuffer.useTexture = this->useTexture;

	this->gEntityPSConstantBuffer.specularHighlight.x = this->entityMaterial.specularPower;
	this->gEntityPSConstantBuffer.specularHighlight.y = this->entityMaterial.specularIntensity;

	// Graphics object will check this if buffer should be updated or not.
	this->shouldUpdateGPUData = true;
}

void Entity::Reset() {
	this->dataChanged = false;
	this->shouldUpdateGPUData = false;
}

// Texture
void Entity::attachTextureAndSampler(Texture* texture, TextureSampler* textureSampler){
	this->texture = texture;
	this->textureSampler = textureSampler;
	this->useTexture = true;

	this->dataChanged = true;
}

void Entity::detachTextureAndSampler() {
	this->texture = NULL;
	this->textureSampler = NULL;
	this->useTexture = false;

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
		this->meshDeformer->skeleton = &this->mesh->gSkeleton;
		this->meshDeformer->Setup();

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
	this->entityMaterial.color = color;
	this->dataChanged = true;
}

void Entity::Translate(Vector3 translationVector){
	this->gPosition.x += translationVector.x;
	this->gPosition.y += translationVector.y;
	this->gPosition.z += translationVector.z;
	this->dataChanged = true;
}

void Entity::rotateQuaternion(Vector4 rotationQuaternionVector){
	this->gRotationQ.x = rotationQuaternionVector.x;
	this->gRotationQ.y = rotationQuaternionVector.y;
	this->gRotationQ.z = rotationQuaternionVector.z;
	this->gRotationQ.w = rotationQuaternionVector.w;

	this->dataChanged = true;
}

void Entity::Scale(Vector3 scalingVector){
	this->gSize.x *= scalingVector.x;
	this->gSize.y *= scalingVector.y;
	this->gSize.z *= scalingVector.z;

	this->dataChanged = true;
}

//// Physics
void Entity::attachCollisionShape(CollisionShape* collisionShape) {
	this->pCollisionShape = collisionShape;
}

void Entity::attachCollisionActor(CollisionActor* collisionActor) {
	this->pCollisionActor = collisionActor;
}