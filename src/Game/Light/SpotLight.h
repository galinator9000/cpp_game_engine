#pragma once
#include "Light/Light.h"
#include "Structs.h"

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;
namespace wrl = Microsoft::WRL;

class SpotLight : public Light {
public:
	SpotLight(Vector3 position, Vector3 direction, float intensity, Color color = Color(1, 1, 1, 1), float halfSpotAngle = dx::XM_PIDIV4);
	void setSpotAngle(float halfSpotAngle);
};