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
		this->specularColor = Color(1, 1, 1, 1);
		this->specularIntensity = 0.18f;
		this->specularPower = 2.0f;
	}

	Material(
		Texture* texture = NULL,
		Color color = Color(1, 1, 1, 1),
		float specularIntensity = 0.18f,
		float specularPower = 2.0f,
		Color specularColor = Color(1, 1, 1, 1)
	) {
		this->texture = texture;
		this->color = color;
		this->specularColor = specularColor;
		this->specularIntensity = specularIntensity;
		this->specularPower = specularPower;

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
		this->gMaterialSConstantBuffer.specularColor = this->specularColor.loadXMFLOAT();
		this->gMaterialSConstantBuffer.specularIntensity = this->specularIntensity;
		this->gMaterialSConstantBuffer.specularPower = this->specularPower;

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
	Color specularColor;
	float specularIntensity;
	float specularPower;

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