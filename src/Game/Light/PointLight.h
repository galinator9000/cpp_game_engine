#pragma once
#include "Light/Light.h"
#include "Structs.h"

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;
namespace wrl = Microsoft::WRL;

class PointLight : public Light {
public:
	PointLight(Vector3 position, float intensity, Color color = Color(1, 1, 1, 1), bool shadowCasting=false);
};