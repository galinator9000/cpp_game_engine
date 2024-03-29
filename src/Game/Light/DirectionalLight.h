#pragma once
#include "Light/Light.h"
#include "Structs.h"

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;
namespace wrl = Microsoft::WRL;

class DirectionalLight : public Light {
public:
	DirectionalLight(Vector3 direction, float intensity, Color color=Color(1, 1, 1, 1), bool isCastingShadow = false, unsigned int CSMLevel = 1);
};