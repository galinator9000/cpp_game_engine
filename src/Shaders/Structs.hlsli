//// Main VertexShader
// Shadow map struct.
struct ShadowMap {
	matrix viewMatrix;
	matrix projectionMatrix;
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