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
#include "Material.h"
#include "CollisionMaterial.h"
#include "../Graphics/Texture.h"
#include "../Graphics/TextureSampler.h"

namespace dx = DirectX;
namespace wrl = Microsoft::WRL;

class Entity {
public:
	// Default constructor.
	Entity();
	Entity(
		Vector3 size = Vector3(1, 1, 1), Vector3 position = Vector3(0, 0, 0), Vector4 rotationQ = Vector4(1, 0, 0, 0),
		Material* material = NULL, CollisionMaterial* collisionMaterial = NULL,
		Mesh* pMesh = NULL,
		CollisionShape* pCollisionShape = NULL,
		CollisionActor* pCollisionActor = NULL
	);
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
	Material* gMaterial = NULL;
	CollisionMaterial* collisionMaterial = NULL;

	// Position & Rotation & Scaling setters.
	void setPosition(Vector3 position);
	void setRotation(Vector4 rotationQ);
	void setScale(Vector3 scaling);

	// Transformations
	void Translate(Vector3 translationVector);
	void rotateQuaternion(Vector4 rotationVector);
	void Scale(Vector3 scalingVector);

	// Material
	void attachMaterial(Material* material) {
		if (material != NULL) {
			this->gMaterial = material;
		}
	}

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
	wrl::ComPtr<ID3D11Buffer> pEntityVSConstantBuffer;
	void updateConstantBuffer();
	bool shouldUpdateGPUData;

	//// Physics
	CollisionShape* pCollisionShape = NULL;
	CollisionActor* pCollisionActor = NULL;
	void attachCollisionShape(CollisionShape* collisionShape);
	void attachCollisionActor(CollisionActor* collisionActor);
};