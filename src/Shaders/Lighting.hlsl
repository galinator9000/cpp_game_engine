//// Constants
// Light type enums.
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

#define MAX_LIGHT_COUNT 8
#define MAX_SHADOW_CASTER_COUNT 4

// Constant identity matrix.
static const matrix identityMatrix = {
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
};

// Attenuation calculation values for Point & Spot light calculation.
static const float attenuation_constant = 1.0f;
static const float attenuation_linear = 0.014;
static const float attenuation_quadratic = 0.0007;
// Global ambient value for diffuse calculation.
static const float4 ambient = float4(0.1f, 0.1f, 0.1f, 0);

//// Pixel Shader buffers & resources.
// Shadow mapping textures.
Texture2D ShadowMapTexture[MAX_SHADOW_CASTER_COUNT] : register(t3);

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
cbuffer LightConstantBuffer : register(b1) {
	Light allLights[MAX_LIGHT_COUNT];
};

SamplerComparisonState shadowPCFSampler {
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;
	ComparisonFunc = LESS_EQUAL;
};

//// Calculations.
// Diffuse calculation.
float4 calculateDiffuse(float4 lightColor, float lightIntensity, float3 lightDirection, float3 normal) {
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
	float3 lightDirection, float3 normal,
	float4 specularHighlightColor, float specularIntensity, float specularPower,
	float3 cameraPosition, float3 pixelPosition
) {
	float3 reflectionVector = reflect(
		lightDirection,
		normal
	);
	return specularHighlightColor * specularIntensity * pow(
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

// Calculates diffuse, specular highlight values of all lights with given position & normal values.
float4 calculateAllLights(
	float3 position,
	float3 normal,
	float3 eyePosition,
	float4 specularHighlightColor,
	float specularIntensity,
	float specularPower
) {
	float4 sumDiffuse = float4(0, 0, 0, 0);
	float4 sumSpecularHighlight = float4(0, 0, 0, 0);

	//// Calculate all lights.
	for (unsigned int light = 0; light < MAX_LIGHT_COUNT; light++) {
		// Undefined lights' intensity is set to -1.
		if (!allLights[light].isActive || allLights[light].intensity <= 0.0f) {
			continue;
		}

		float lightIntensity = allLights[light].intensity;

		// Vertex and Light vector calculations.
		float3 vertexToLight = float3(0, 0, 0);
		float distVertexToLight = 0;
		float3 dirVertexToLight = float3(0, 0, 0);

		float attenuation = 1;
		if (
			allLights[light].type == POINT_LIGHT ||
			allLights[light].type == SPOT_LIGHT
			) {
			vertexToLight = allLights[light].position - position;
			distVertexToLight = length(vertexToLight);
			dirVertexToLight = normalize(vertexToLight);

			attenuation = calculateAttenuation(distVertexToLight);
		}

		switch (allLights[light].type) {
			// Directional Light calculation.
			case DIRECTIONAL_LIGHT:
				sumDiffuse += attenuation * calculateDiffuse(allLights[light].color, lightIntensity, allLights[light].direction, normal);
				sumSpecularHighlight += attenuation * calculateSpecularHighlight(
					allLights[light].direction, normal,
					specularHighlightColor, specularIntensity, specularPower,
					eyePosition, position
				);
				break;
			// Point Light calculation.
			case POINT_LIGHT:
				sumDiffuse += attenuation * calculateDiffuse(allLights[light].color, lightIntensity, -dirVertexToLight, normal);
				sumSpecularHighlight += attenuation * calculateSpecularHighlight(
					-dirVertexToLight, normal,
					specularHighlightColor, specularIntensity, specularPower,
					eyePosition, position
				);
				break;

			// Spot Light calculation.
			case SPOT_LIGHT:
				attenuation *= calculateConeCenterDistance(allLights[light].halfSpotAngle, allLights[light].direction, dirVertexToLight);

				sumDiffuse += attenuation * calculateDiffuse(allLights[light].color, lightIntensity, -dirVertexToLight, normal);
				sumSpecularHighlight += attenuation * calculateSpecularHighlight(
					-dirVertexToLight, normal,
					specularHighlightColor, specularIntensity, specularPower,
					eyePosition, position
				);
				break;
		}
	}

	sumDiffuse = saturate(sumDiffuse);
	sumSpecularHighlight = saturate(sumSpecularHighlight);

	// Set alpha values to 1.
	sumDiffuse.a = 1.0f;
	sumSpecularHighlight.a = 1.0f;

	return (sumDiffuse + sumSpecularHighlight + ambient);
}