// Light type enums.
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

#define MAX_LIGHT_COUNT 8

// Attenuation calculation values for Point & Spot light calculation.
static const float attenuation_constant = 1.0f;
static const float attenuation_linear = 0.014;
static const float attenuation_quadratic = 0.0007;

// Global ambient value for diffuse calculation.
static const float4 ambient = float4(0.03f, 0.03f, 0.03f, 0);

// Light intensity, direction and position values.
struct Light {
	float4 color;
	float intensity;
	float3 position;
	float3 direction;
	unsigned int type;
	float halfSpotAngle;
	float3 padding;
};

// Diffuse calculation.
float4 calculateDiffuse(float4 lightColor, float lightIntensity, float3 lightDirection, float3 normal ) {
	return lightColor * lightIntensity * max(0.0f, dot(-lightDirection, normal));
}

// Attenuation for positional lights.
float calculateAttenuation(float distVertexToLight) {
	return 1.0f / (
		attenuation_constant +
		(attenuation_linear * distVertexToLight) +
		(attenuation_quadratic * (distVertexToLight * distVertexToLight))
	);
}

// Specular highlight calculation.
float4 calculateSpecularHighlight(
	float4 lightColor, float lightIntensity, float3 lightDirection,
	float3 normal, float attenuation,
	float specularIntensity, float specularPower,
	float3 cameraPosition, float3 pixelPosition
) {
	float3 reflectionVector = reflect(
		lightDirection,
		normal
	);
	return lightColor * lightIntensity * attenuation * specularIntensity * pow(
		max(
			0.0f,
			dot(
				normalize(reflectionVector),
				normalize(cameraPosition - pixelPosition)
			)
		),
		specularPower
	);
}

// Cone center calculation of the Spot light.
float calculateConeCenterDistance(float halfSpotAngle, float3 lightDirection, float3 directionVertexToLight) {
	float minCos = cos(halfSpotAngle);
	float maxCos = (minCos + 1.0f) / 2.0f;
	float cosAngle = dot(-lightDirection, directionVertexToLight);
	return smoothstep(minCos, maxCos, cosAngle);
}