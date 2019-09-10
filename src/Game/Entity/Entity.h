#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "PxPhysicsAPI.h"
#include <wrl.h>
#include <vector>
#include <algorithm>

#include "Mesh/Mesh.h"
#include "Mesh/MeshDeformer/MeshDeformer.h"
#include "Collision/CollisionShape.h"
#include "Collision/CollisionActor.h"
#include "Structs.h"
#include "Texture.h"
#include "TextureSampler.h"

using namespace physx;
namespace dx = DirectX;
namespace wrl = Microsoft::WRL;

const enum ENTITY_TYPE{
	BOX,
	PLANE,
	TRIANGLE_MESH
};

class Entity {
public:
	// Default constructor.
	Entity();
	Entity(
		Vector3 size, Vector3 position, Vector4 rotationQ,
		Color color, Vector3 collisionMaterial,
		Mesh* mesh = NULL,
		CollisionShape* pCollisionShape = NULL,
		CollisionActor* pCollisionActor = NULL
	);
	virtual void Update();
	virtual void Reset();
	virtual void setColor(Color color);

	// Derived classes will set these values.
	unsigned int id;
	bool isDrawable = false;
	bool dataChanged;

	//// Graphics
	dx::XMFLOAT3 gSize;
	dx::XMFLOAT3 gPosition;
	dx::XMFLOAT4 gRotationQ;
	EntityMaterial entityMaterial;
	CollisionMaterial collisionMaterial;

	void Translate(Vector3 translationVector);
	void rotateQuaternion(Vector4 rotationVector);
	void Scale(Vector3 scalingVector);

	// Texture
	Texture* texture = NULL;
	TextureSampler* textureSampler = NULL;
	virtual void attachTextureAndSampler(Texture* texture, TextureSampler* textureSampler);
	virtual void detachTextureAndSampler();
	bool useTexture = false;

	// Hierarchy system
	Entity* parentEntity = NULL;
	std::vector<Entity*> childEntities;
	void attachChild(Entity* child);
	void detachChild(Entity* child);
	Entity* getChild(int childIndex);
	unsigned int getChildCount();

	// Mesh
	Mesh* mesh = NULL;
	virtual bool attachMesh(Mesh* entityMesh);

	// Mesh deformer
	MeshDeformer* meshDeformer = NULL;
	bool attachMeshDeformer(MeshDeformer* meshDeformer);
	bool setAnimation(const char* animName);
	bool useMeshDeformer = false;

	// Constant buffer
	EntityVSConstantBuffer gEntityVSConstantBuffer;
	EntityPSConstantBuffer gEntityPSConstantBuffer;
	virtual void updateConstantBuffer();
	bool shouldUpdateGPUData;

	wrl::ComPtr<ID3D11Buffer> pEntityVSConstantBuffer;
	wrl::ComPtr<ID3D11Buffer> pEntityPSConstantBuffer;

	//// Physics
	CollisionShape* pCollisionShape = NULL;
	CollisionActor* pCollisionActor = NULL;
	virtual void attachCollisionShape(CollisionShape* collisionShape);
	virtual void attachCollisionActor(CollisionActor* collisionActor);
};