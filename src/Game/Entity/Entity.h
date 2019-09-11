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
#include "Texture.h"
#include "TextureSampler.h"

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
	Entity(EntityProperties entityProperties);
	void Update();
	virtual void Reset();
	void setColor(Color color);

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

	// Transformations
	void Translate(Vector3 translationVector);
	void rotateQuaternion(Vector4 rotationVector);
	void Scale(Vector3 scalingVector);

	// Texture
	Texture* texture = NULL;
	TextureSampler* textureSampler = NULL;
	void attachTextureAndSampler(Texture* texture, TextureSampler* textureSampler);
	void detachTextureAndSampler();
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