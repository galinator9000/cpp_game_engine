#pragma once
#include "Constants.h"
#include <DirectXMath.h>
#include "PxPhysicsAPI.h"
using namespace physx;
namespace dx = DirectX;

struct GraphicsSettingsStruct {
	int PCFLevel = 0;
	dx::XMFLOAT3 padding;
};

//// Constant buffers that provided to Shaders.
// Entity constant buffer for Vertex Shader.
struct EntityVSConstantBuffer {
	// Transformation matrix for entity.
	dx::XMFLOAT4X4 worldMatrix;
	bool useMeshDeformer;
	dx::XMFLOAT3 padding;
};

// Material constant buffer for providing it to shader(s).
struct MaterialSConstantBuffer {
	dx::XMFLOAT4 color;
	dx::XMFLOAT4 specularHighlightColor;
	unsigned int useDiffuse;
	unsigned int useNormalMapping;
	unsigned int useAlpha;
	unsigned int useSpecular;
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
struct ShadowMapSStruct {
	dx::XMFLOAT4X4 projectionMatrix;
	dx::XMFLOAT4X4 viewMatrix;
	float activeCameraSubfrustumNearPlaneDistance;
	float activeCameraSubfrustumFarPlaneDistance;
	dx::XMFLOAT2 padding;
};

struct ShadowBoxSStruct {
	ShadowMapSStruct shadowMap[MAX_SHADOWMAP_COUNT];
	unsigned int isActive;
	unsigned int lightType;
	unsigned int lightID;
	float shadowDistance;
	unsigned int shadowMapCount;
	dx::XMFLOAT3 padding;
};