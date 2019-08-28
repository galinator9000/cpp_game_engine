#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "PxPhysicsAPI.h"
#include <wrl.h>
#include <vector>
#include <algorithm>

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

class BaseEntity{
public:
	~BaseEntity();
	virtual void Update(){};
	virtual void Reset();
	virtual void setColor(Color color);

	// Derived classes will set these values.
	unsigned int id;
	unsigned int type;
	bool isDynamic;

	//// Physics
	static PxPhysics* ppxPhysics;
	PxActor* pActor = NULL;
	PxShape* pShape = NULL;

	//// Graphics
	dx::XMFLOAT3 gSize;
	dx::XMFLOAT3 gPosition;
	dx::XMFLOAT4 gRotationQ;
	dx::XMFLOAT3 gColor;

	// Texture
	Texture* texture;
	TextureSampler* textureSampler;
	virtual void attachTextureAndSampler(Texture* texture, TextureSampler* textureSampler);
	virtual void detachTextureAndSampler();
	bool useTexture = false;

	// Vertex & index
	Vertex* gVertices;
	unsigned int* gIndices;
	unsigned int gVertexCount;
	unsigned int gIndexCount;
	virtual void gCreateVerticesAndIndices(){};

	// Constant buffer
	EntityVSConstantBuffer gEntityVSConstantBuffer;
	EntityPSConstantBuffer gEntityPSConstantBuffer;
	virtual void updateConstantBuffer();
	bool shouldUpdateGPUData;
	bool dataChanged;

	wrl::ComPtr<ID3D11Buffer> pEntityVSConstantBuffer;
	wrl::ComPtr<ID3D11Buffer> pEntityPSConstantBuffer;
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
};
