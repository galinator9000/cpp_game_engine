#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <string>

#include "Structs.h"

namespace dx = DirectX;
namespace wrl = Microsoft::WRL;

class Texture{
public:
	Texture(const char* textureReferenceName, const char* fileName);

	std::wstring textureReferenceName;
	std::wstring fileName;

	wrl::ComPtr<ID3D11Resource> pResource;
	wrl::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
};