#pragma once
#include <DirectXMath.h>
#include "PxPhysicsAPI.h"
using namespace physx;
namespace dx = DirectX;

#define MAX_JOINT_COUNT 256
#define MAX_JOINT_PER_VERTEX 4

//// Constant buffers that provided to Shaders.
// Entity constant buffer for Vertex Shader.
struct EntityVSConstantBuffer {
	// Transformation matrix for entity.
	dx::XMFLOAT4X4 worldMatrix;
	bool useMeshDeformer;
	dx::XMFLOAT3 padding;
};

// Entity constant buffer for Pixel Shader.
struct EntityPSConstantBuffer {
	dx::XMFLOAT4 color;
	dx::XMFLOAT4 specularHighlightColor;
	float specularIntensity;
	float specularPower;
	unsigned int useTexture;
	unsigned int useNormalMapping;
	unsigned int useAlpha;
	dx::XMFLOAT3 padding1;
};

// Mesh Deformer constant buffer for Vertex Shader.
struct MeshDeformerVSConstantBuffer {
	dx::XMFLOAT4X4 jointsTransformMatrix[MAX_JOINT_COUNT];
};

// Each light object holds this, light's intensity, direction and position values.
struct LightPSStruct {
	dx::XMFLOAT4 color;
	dx::XMFLOAT3 position;
	float intensity;
	dx::XMFLOAT3 direction;
	unsigned int isActive;
	unsigned int type;
	float halfSpotAngle;
	dx::XMFLOAT2 padding;
};

// Each camera object holds this, camera's position and projection information held.
struct CameraVSConstantBuffer {
	dx::XMFLOAT4X4 viewMatrix;
	dx::XMFLOAT4X4 projectionMatrix;
	dx::XMFLOAT3 cameraPosition;
	float padding;
};

// Each camera object holds this, camera's position and projection information held.
struct ShadowMapVSStruct {
	dx::XMFLOAT4X4 viewMatrix;
	dx::XMFLOAT4X4 projectionMatrix;
};