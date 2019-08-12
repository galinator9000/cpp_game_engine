#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "PxPhysicsAPI.h"
#include <wrl.h>

#include "Structs.h"
#include <vector>

using namespace physx;
namespace dx = DirectX;
namespace wrl = Microsoft::WRL;

const enum ENTITY_TYPE{
	BOX,
	PLANE
};

class BaseEntity{
public:
	// Derived classes will set this value.
	unsigned int id;
	unsigned int type;

	// Physics
	static PxPhysics* ppxPhysics;
	PxShape* pShape = NULL;
	PxActor* pActor = NULL;

	// Graphics
	dx::XMFLOAT3 gSize;
	dx::XMFLOAT3 gPosition;
	dx::XMFLOAT3 gRotation;

	dx::XMFLOAT4X4 transformMatrix;

	Vertex* vertices;
	unsigned short* indices;
	unsigned int vertexCount;
	unsigned int indexCount;
	void createVertexAndIndexBuffer();

	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
};
