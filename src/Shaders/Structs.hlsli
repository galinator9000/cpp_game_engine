#include "Constants.hlsli"

//// Main VertexShader
// Shadow map struct.
struct ShadowMapSubfrustum {
	matrix projectionMatrix;
};

struct ShadowMap {
	ShadowMapSubfrustum subfrustum[MAX_CSM_SUBFRUSTUM_COUNT];
	matrix viewMatrix;
	bool isActive;
	unsigned int lightType;
	float shadowDistance;
	unsigned int lightID;
};

//// Main PixelShader

//// Lighting
// Light intensity, direction and position values.
struct Light {
	float4 color;
	float3 position;
	float intensity;
	float3 direction;
	unsigned int isActive;
	unsigned int type;
	float halfSpotAngle;
	float2 padding;
};