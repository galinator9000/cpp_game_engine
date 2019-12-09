#pragma once
#include "Structs.h"
#include "../Graphics/Texture.h"

#include <wrl.h>

namespace wrl = Microsoft::WRL;

class Material {
public:
	Material() {
		this->texture = NULL;
		this->color = Color(1, 1, 1, 1);
		this->specularHighlightColor = Color(0.18f, 0.18f, 0.18f);
	}

	Material(
		Texture* texture = NULL,
		Color color = Color(1, 1, 1, 1),
		Color specularHighlightColor = Color(0.18f, 0.18f, 0.18f)
	) {
		this->texture = texture;
		this->color = color;
		this->specularHighlightColor = specularHighlightColor;

		this->updateConstantBuffer();
	}

	void Update() {
		if (this->dataChanged) {
			this->updateConstantBuffer();
			this->dataChanged = false;
		}
	}

	void updateConstantBuffer() {
		this->gMaterialSConstantBuffer.color = this->color.loadXMFLOAT();
		this->gMaterialSConstantBuffer.specularHighlightColor = this->specularHighlightColor.loadXMFLOAT();

		// Texture
		if (texture != NULL) {
			this->gMaterialSConstantBuffer.useDiffuse = (unsigned int) this->texture->useDiffuse;
			this->gMaterialSConstantBuffer.useNormalMapping = (unsigned int) this->texture->useNormalMapping;
			this->gMaterialSConstantBuffer.useSpecular = (unsigned int) this->texture->useSpecular;
			this->gMaterialSConstantBuffer.useAlpha = (unsigned int) this->texture->useAlpha;
		}

		this->shouldUpdateGPUData = true;
	}

	Texture* texture = NULL;
	Color color;
	Color specularHighlightColor;

	void attachTexture(Texture* texture) {
		if (texture != NULL) {
			this->texture = texture;
			dataChanged = true;
		}
	}

	// Constant buffer.
	MaterialSConstantBuffer gMaterialSConstantBuffer;
	wrl::ComPtr<ID3D11Buffer> pMaterialSConstantBuffer;
	bool dataChanged = false;
	bool shouldUpdateGPUData = false;
};