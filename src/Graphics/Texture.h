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
	Texture(unsigned int width, unsigned int height){
		this->gWidth = width;
		this->gHeight = height;
	}
	Texture(
		const char* textureReferenceName,
		const char* diffuseFileName,
		const char* normalFileName = NULL,
		const char* specularFileName = NULL,
		const char* alphaFileName = NULL
	) {
		if (textureReferenceName == NULL || diffuseFileName == NULL) {
			return;
		}

		this->textureReferenceName = std::string(textureReferenceName);

		if (diffuseFileName != NULL) {
			std::string colorFileName_(diffuseFileName);
			this->diffuseFileName = std::wstring(colorFileName_.begin(), colorFileName_.end());
			this->useDiffuse = true;
		}

		if (normalFileName != NULL) {
			std::string normalFileName_(normalFileName);
			this->normalFileName = std::wstring(normalFileName_.begin(), normalFileName_.end());
			this->useNormalMapping = true;
		}

		if (specularFileName != NULL) {
			std::string specularFileName_(specularFileName);
			this->specularFileName = std::wstring(specularFileName_.begin(), specularFileName_.end());
			this->useSpecular = true;
		}

		if (alphaFileName != NULL) {
			std::string alphaFileName_(alphaFileName);
			this->alphaFileName = std::wstring(alphaFileName_.begin(), alphaFileName_.end());
			this->useAlpha = true;
		}
	}

	// Texture name.
	std::string textureReferenceName;

	//// Custom
	unsigned int gWidth;
	unsigned int gHeight;

	//// DDS
	// File names.
	std::wstring diffuseFileName;
	std::wstring normalFileName;
	std::wstring specularFileName;
	std::wstring alphaFileName;

	// Diffuse
	wrl::ComPtr<ID3D11Resource> pDiffuseResource;
	wrl::ComPtr<ID3D11ShaderResourceView> pDiffuseShaderResourceView;
	bool useDiffuse = false;

	// Normal
	wrl::ComPtr<ID3D11Resource> pNormalResource;
	wrl::ComPtr<ID3D11ShaderResourceView> pNormalShaderResourceView;
	bool useNormalMapping = false;

	// Specular
	wrl::ComPtr<ID3D11Resource> pSpecularResource;
	wrl::ComPtr<ID3D11ShaderResourceView> pSpecularShaderResourceView;
	bool useSpecular = false;

	// Alpha
	wrl::ComPtr<ID3D11Resource> pAlphaResource;
	wrl::ComPtr<ID3D11ShaderResourceView> pAlphaShaderResourceView;
	bool useAlpha = false;
};