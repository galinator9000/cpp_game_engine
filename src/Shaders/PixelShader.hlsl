#include "Lighting.hlsl"

// Provided by entity object.
Texture2D Texture : register(t0);
Texture2D NormalMappingTexture : register(t1);
Texture2D AlphaTexture : register(t2);

// Various samplers.
SamplerState defaultSampler : register(s0);
SamplerState clampSampler : register(s1);
SamplerState whiteBorderSampler : register(s2);

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

	// Camera position
	float3 eyePosition : EyePosition;

	// Shadow map
	// XY, shadow map texture UV coordinates
	// Z, distance from light.
	float4 shadowMapPosition[MAX_SHADOW_CASTER_COUNT] : TEXCOORD0;
};

// Output structure of the Pixel shader.
struct PSOut {
	float4 color : SV_Target;
};

PSOut main(PSIn psIn){
	PSOut psOut;

	// Normalizing normal input vectors because Rasterizer stage interpolates them.
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

	LightingOutput lightingOutput = calculateAllLights(
		psIn.positionPS, psIn.normal, psIn.eyePosition,
		specularHighlightColor, specularIntensity, specularPower
	);

	// Calculate shadow map texture UV coordinates.
	float2 shadowMapCoords;
	shadowMapCoords.x = psIn.shadowMapPosition[0].x / psIn.shadowMapPosition[0].w * 0.5 + 0.5;
	shadowMapCoords.y = -psIn.shadowMapPosition[0].y / psIn.shadowMapPosition[0].w * 0.5 + 0.5;
	float finalDepth = (psIn.shadowMapPosition[0].z / psIn.shadowMapPosition[0].w) - 0.001f;

	if ((saturate(shadowMapCoords.x) == shadowMapCoords.x) && (saturate(shadowMapCoords.y) == shadowMapCoords.y)) {
		//// Process shadow maps.
		/*for (unsigned int sc = 0; sc < MAX_SHADOW_CASTER_COUNT; sc++) {
			float sampledDepth = ShadowMapTexture[sc].Sample(ShadowMapSampler[sc], psIn.shadowMapCoord[sc]).r;

			if (psIn.finalDepth[sc] > sampledDepth) {
				sumDiffuse = sumDiffuse * 0.4f;
			}
		}*/

		// PCF shadow
		/*float shadowFactor = ShadowMapTexture[0].SampleCmpLevelZero(shadowPCFSampler, shadowMapCoords, finalDepth);
		sumDiffuse = sumDiffuse * (1-shadowFactor);*/

		float sampledDepth = ShadowMapTexture[0].Sample(whiteBorderSampler, shadowMapCoords).r;
		if (finalDepth > sampledDepth) {
			lightingOutput.diffuse = lightingOutput.diffuse * float4(0.4f, 0.4f, 0.4f, 1);
		}
	}

	// Use solid color of entity or texture?
	float4 texture_or_solid = entityColor;
	if (useTexture) {
		texture_or_solid = Texture.Sample(defaultSampler, psIn.texture_UV);
	}

	// Set alpha values to 1.
	lightingOutput.diffuse.a = 1.0f;
	lightingOutput.specularHighlight.a = 1.0f;

	// If uses alpha value from texture, sample it. 
	if (useNormalMapping) {
		float3 alpha = AlphaTexture.Sample(defaultSampler, psIn.texture_UV).rgb;

		texture_or_solid.a = (alpha.r + alpha.g + alpha.b) / 3;
		lightingOutput.specularHighlight.a = (alpha.r + alpha.g + alpha.b) / 3;
		lightingOutput.diffuse.a = (alpha.r + alpha.g + alpha.b) / 3;
	}else {
		texture_or_solid.a = 1.0f;
	}

	// Add ambient light & blend the color of the entity.
	psOut.color = float4(
		(lightingOutput.diffuse + ambient) * (texture_or_solid + lightingOutput.specularHighlight)
	);

	return psOut;
}