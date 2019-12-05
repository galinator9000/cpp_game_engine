#include "Lighting.hlsl"

// Various samplers.
SamplerState defaultSampler : register(s0);
SamplerState clampSampler : register(s1);

// Provided by entity object.
Texture2D Texture : register(t0);
Texture2D NormalMappingTexture : register(t1);
Texture2D AlphaTexture : register(t2);

cbuffer EntityPSConstantBuffer : register(b0) {
	float4 entityColor;
	float4 specularHighlightColor;
	float specularIntensity;
	float specularPower;
	bool useTexture;
	bool useNormalMapping;
	bool useAlpha;
	float3 padding;
};

// Input structure of the Pixel shader.
struct PSIn {
	// Vertex values
	float3 positionPS : Position;
	float2 texture_UV : TextureUV;
	float3 normal : Normal;
	float3 tangent : Tangent;
	float3 binormal : Binormal;

	// Camera position in world space.
	float3 eyePosition : EyePosition;

	// Final vertex shader output
	float4 position : SV_Position;
};

// Output structure of the Pixel shader.
struct PSOut {
	float4 color : SV_Target;
};

PSOut main(PSIn psIn){
	PSOut psOut;

	// Current pixel color: Use solid entity color, or sample from texture if entity provides it.
	float4 pixelColor = entityColor;
	if (useTexture) {
		pixelColor = Texture.Sample(defaultSampler, psIn.texture_UV);
	}

	psIn.normal = normalize(psIn.normal);
	// Sample from normal map texture if entity uses it.
	if (useNormalMapping) {
		psIn.tangent = normalize(psIn.tangent);
		psIn.binormal = normalize(psIn.binormal);

		// Sample normal vector from texture, these values ranges 0 to 1,
		// we need to expand and reposition them as -1 to 1.
		float4 sampledNormal = NormalMappingTexture.Sample(defaultSampler, psIn.texture_UV);
		sampledNormal = sampledNormal * 2.0f - 1.0f;

		// Tangent space to object space.
		psIn.normal = mul(
			sampledNormal.xyz,
			float3x3(
				psIn.tangent,
				psIn.binormal,
				psIn.normal
			)
		);
	}

	// If uses alpha value from texture, sample it.
	if (useAlpha) {
		float3 alpha = AlphaTexture.Sample(defaultSampler, psIn.texture_UV).rgb;
		pixelColor.a = (alpha.r + alpha.g + alpha.b) / 3;
	}
	else {
		pixelColor.a = 1.0f;
	}

	//// Calculate lighting.
	float4 lightingFactor = calculateAllLights(
		psIn.positionPS, psIn.normal, psIn.eyePosition,
		specularHighlightColor, specularIntensity, specularPower
	);

	//// Calculate shadows.
	// Variables to use on for-loops.
	unsigned int sb = 0;	// ShadowBox index
	unsigned int sm = 0;	// ShadowMap index

	// Shadow map
	float4 shadowMapPosition[MAX_SHADOWBOX_COUNT * MAX_SHADOWMAP_COUNT];
	// XY, shadow map texture UV coordinates
	// Z, distance from light.
	float4 shadowMapPositionCurrent = float4(0, 0, 0, 1);
	[unroll]
	for (sb = 0; sb < MAX_SHADOWBOX_COUNT; sb++) {
		if (shadowBoxes[sb].isActive) {
			[unroll]
			for (sm = 0; sm < MAX_SHADOWMAP_COUNT; sm++) {
				// Apply shadow map projection & view matrices.
				shadowMapPositionCurrent = mul(float4(psIn.positionPS, 1), shadowBoxes[sb].shadowMap[sm].viewMatrix);
				shadowMapPositionCurrent = mul(shadowMapPositionCurrent, shadowBoxes[sb].shadowMap[sm].projectionMatrix);
				shadowMapPosition[sb * MAX_SHADOWMAP_COUNT + sm] = shadowMapPositionCurrent;
			}
		}
	}

	// Specify which shadow map to sample from.
	unsigned int shadowMapIndices[MAX_SHADOWBOX_COUNT];
	float distanceToPixel = distance(psIn.eyePosition, psIn.positionPS);
	[unroll]
	for (sb = 0; sb < MAX_SHADOWBOX_COUNT; sb++) {
		if (shadowBoxes[sb].isActive && shadowBoxes[sb].lightType == DIRECTIONAL_LIGHT) {
			[unroll]
			for (sm = 0; sm < MAX_SHADOWMAP_COUNT; sm++) {
				if (distanceToPixel <= shadowBoxes[sb].shadowMap[sm].activeCameraSubfrustumFarPlaneDistance) {
					shadowMapIndices[sb] = sm;
					break;
				}
			}
		}
	}

	float shadowFactor = calculateAllShadows(
		shadowMapPosition,
		shadowMapIndices,
		psIn.positionPS,
		psIn.eyePosition
	);

	// Apply shadow.
	lightingFactor = float4((lightingFactor * (1 - shadowFactor)).rgb, 1);

	// Final result: Combine light & pixel color.
	psOut.color = float4((lightingFactor + ambientDiffuseLightGlobal) * pixelColor);

	return psOut;
}