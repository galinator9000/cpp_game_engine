#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "PxPhysicsAPI.h"
#include <wrl.h>
#include <vector>
#include <algorithm>

#include "Structs.h"

using namespace physx;
namespace dx = DirectX;
namespace wrl = Microsoft::WRL;

const enum ENTITY_TYPE{
	BOX,
	PLANE
};

class BaseEntity{
public:
	~BaseEntity();
	virtual void Update(bool initial=false);

	// Derived classes will set this value.
	unsigned int id;
	unsigned int type;
	bool isDynamic;

	// Physics
	static PxPhysics* ppxPhysics;
	PxActor* pActor = NULL;
	PxShape* pShape = NULL;

	// Graphics
	dx::XMFLOAT3 gSize;
	dx::XMFLOAT3 gPosition;
	dx::XMFLOAT4 gRotationQ;

	// Vertex & index
	Vertex* gVertices;
	unsigned short* gIndices;
	unsigned int gVertexCount;
	unsigned int gIndexCount;
	virtual void gCreateVerticesAndIndices();

	// Constant buffer
	ConstantBuffer gConstBuffer;
	virtual void updateConstantBuffer();
	bool shouldUpdateData;

	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
};
