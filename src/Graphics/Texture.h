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
	Texture(){}
	Texture(
		const char* textureReferenceName,
		const char* colorFileName,
		const char* normalFileName = NULL,
		const char* alphaFileName = NULL
	) {
		if (textureReferenceName == NULL || colorFileName == NULL) {
			return;
		}

		this->textureReferenceName = std::string(textureReferenceName);

		std::string colorFileName_(colorFileName);
		this->colorFileName = std::wstring(colorFileName_.begin(), colorFileName_.end());

		if (normalFileName != NULL) {
			std::string normalFileName_(normalFileName);
			this->normalFileName = std::wstring(normalFileName_.begin(), normalFileName_.end());
			this->useNormalMapping = true;
		}

		if (alphaFileName != NULL) {
			std::string alphaFileName_(alphaFileName);
			this->alphaFileName = std::wstring(alphaFileName_.begin(), alphaFileName_.end());
			this->useAlpha = true;
		}
	}

	// Texture name.
	std::string textureReferenceName;

	// File names.
	std::wstring colorFileName;
	std::wstring alphaFileName;
	std::wstring normalFileName;

	// Color
	wrl::ComPtr<ID3D11Resource> pColorResource;
	wrl::ComPtr<ID3D11ShaderResourceView> pColorShaderResourceView;

	// Alpha
	wrl::ComPtr<ID3D11Resource> pAlphaResource;
	wrl::ComPtr<ID3D11ShaderResourceView> pAlphaShaderResourceView;
	bool useAlpha = false;

	// Normal
	wrl::ComPtr<ID3D11Resource> pNormalResource;
	wrl::ComPtr<ID3D11ShaderResourceView> pNormalShaderResourceView;
	bool useNormalMapping = false;
};