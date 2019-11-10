#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <algorithm>

#include "Mesh/Mesh.h"
#include "Mesh/MeshDeformer/MeshDeformer.h"
#include "Collision/CollisionShape.h"
#include "Collision/CollisionActor.h"
#include "Structs.h"
#include "../Graphics/Texture.h"
#include "../Graphics/TextureSampler.h"

namespace dx = DirectX;
namespace wrl = Microsoft::WRL;

class Entity {
public:
	// Default constructor.
	Entity();
	Entity(EntityProperties entityProperties);
	void Update();
	virtual void Reset();
	void setColor(Color color);

	// Derived classes will set these values.
	unsigned int id;
	bool isDrawable = false;
	bool dataChanged;

	//// Graphics
	Vector3 gSize;
	Vector3 gPosition;
	Vector4 gRotationQ;
	Material material;
	CollisionMaterial collisionMaterial;

	// Position & Rotation & Scaling setters.
	void setPosition(Vector3 position);
	void setRotation(Vector4 rotationQ);
	void setScale(Vector3 scaling);

	// Transformations
	void Translate(Vector3 translationVector);
	void rotateQuaternion(Vector4 rotationVector);
	void Scale(Vector3 scalingVector);

	// Texture
	Texture* texture = NULL;
	TextureSampler* textureSampler = NULL;
	void attachTexture(Texture* texture);
	void attachTextureSampler(TextureSampler* textureSampler);
	bool useTexture = false;

	// Normal map
	Texture* normalMappingTexture = NULL;
	void attachNormalMappingTexture(Texture* normalMappingTexture);
	bool useNormalMapping = false;

	// Hierarchy system
	Entity* parentEntity = NULL;
	std::vector<Entity*> childEntities;
	void attachChild(Entity* child);
	void detachChild(Entity* child);
	Entity* getChild(int childIndex);
	unsigned int getChildCount();

	// Mesh
	Mesh* mesh = NULL;
	bool attachMesh(Mesh* entityMesh);

	// Mesh deformer
	MeshDeformer* meshDeformer = NULL;
	bool attachMeshDeformer(MeshDeformer* meshDeformer);
	bool setAnimation(const char* animName);
	bool useMeshDeformer = false;

	// Constant buffer
	EntityVSConstantBuffer gEntityVSConstantBuffer;
	EntityPSConstantBuffer gEntityPSConstantBuffer;
	void updateConstantBuffer();
	bool shouldUpdateGPUData;

	wrl::ComPtr<ID3D11Buffer> pEntityVSConstantBuffer;
	wrl::ComPtr<ID3D11Buffer> pEntityPSConstantBuffer;

	//// Physics
	CollisionShape* pCollisionShape = NULL;
	CollisionActor* pCollisionActor = NULL;
	void attachCollisionShape(CollisionShape* collisionShape);
	void attachCollisionActor(CollisionActor* collisionActor);
};