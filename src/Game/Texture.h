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

class Texture{
public:
	Texture(const char* textureReferenceName, const char* fileName);

	wrl::ComPtr<ID3D11Texture2D> pTexture;
	wrl::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	wrl::ComPtr<ID3D11Resource> pResource;
};