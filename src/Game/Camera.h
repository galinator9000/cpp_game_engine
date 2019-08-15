#pragma once
#include "Entity/Structs.h"

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

class Camera{
public:
	Camera(float posX, float posY, float posZ, unsigned int fov, float aspectRatio);
	void Update(bool initial=false);
	void updateConstantBuffer();

	unsigned int id;
	unsigned int gFieldOfView;
	float gAspectRatio;

	// GPU side
	ViewProjectionMatrices gViewProjection;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pViewProjectionBuffer;
	bool shouldUpdateData = false;
	bool hasChanged = false;

	dx::XMFLOAT3 eye;
	dx::XMFLOAT3 at;
	dx::XMFLOAT3 up;
};