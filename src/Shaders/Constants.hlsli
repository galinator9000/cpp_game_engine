// Constant identity matrix.
static const matrix identityMatrix = {
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
};

//// Main VertexShader
#define MAX_JOINT_COUNT 256
#define MAX_JOINT_PER_VERTEX 4

//// Main PixelShader

//// Lighting
// Light type enums.
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

#define MAX_LIGHT_COUNT 8
#define MAX_SHADOW_CASTER_COUNT 2
#define MAX_CSM_SUBFRUSTUM_COUNT 3

// Attenuation calculation values for Point & Spot light calculation.
static const float attenuation_constant = 1.0f;
static const float attenuation_linear = 0.014;
static const float attenuation_quadratic = 0.0007;

// Global ambient value for diffuse calculation.
static const float4 ambientDiffuseLightGlobal = float4(0.25f, 0.25f, 0.25f, 0);