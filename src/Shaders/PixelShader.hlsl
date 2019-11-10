#include "Lighting.hlsl"

// Provided by entity object.
Texture2D Texture : register(t0);
Texture2D NormalMappingTexture : register(t1);
SamplerState Sampler : register(s0);

// Shadow mapping texture & samplers.
Texture2D ShadowMapTexture[MAX_SHADOW_CASTER_COUNT] : register(t2);
SamplerState ShadowMapSampler[MAX_SHADOW_CASTER_COUNT] : register(s1);

cbuffer EntityPSConstantBuffer : register(b0) {
	float4 entityColor;
	float4 specularHighlightColor;
	float specularIntensity;
	float specularPower;
	bool useTexture;
	bool useNormalMapping;
};

cbuffer LightConstantBuffer : register(b1) {
	Light allLights[MAX_LIGHT_COUNT];
};

// Constant identity matrix.
static const matrix identityMatrix = {
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
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
	float4 shadowMapPosition[MAX_SHADOW_CASTER_COUNT] : ShadowMapPosition;
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
		float4 sampledNormal = NormalMappingTexture.Sample(Sampler, psIn.texture_UV);
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

	float4 sumDiffuse;
	float4 sumSpecular;

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
			vertexToLight = allLights[light].position - psIn.positionPS;
			distVertexToLight = length(vertexToLight);
			dirVertexToLight = normalize(vertexToLight);

			attenuation = calculateAttenuation(distVertexToLight);
		}
		
		switch (allLights[light].type) {
			// Directional Light calculation.
			case DIRECTIONAL_LIGHT:
				sumDiffuse += calculateDiffuse(allLights[light].color, lightIntensity, allLights[light].direction, psIn.normal);
				sumSpecular += attenuation * calculateSpecularHighlight(
					allLights[light].direction, psIn.normal,
					specularHighlightColor, specularIntensity, specularPower,
					psIn.eyePosition.xyz, psIn.positionPS
				);
				break;
			// Point Light calculation.
			case POINT_LIGHT:
				sumDiffuse += attenuation * calculateDiffuse(allLights[light].color, lightIntensity, -dirVertexToLight, psIn.normal);
				sumSpecular += attenuation * calculateSpecularHighlight(
					-dirVertexToLight, psIn.normal,
					specularHighlightColor, specularIntensity, specularPower,
					psIn.eyePosition.xyz, psIn.positionPS
				);
				break;

			// Spot Light calculation.
			case SPOT_LIGHT:
				lightIntensity *= calculateConeCenterDistance(allLights[light].halfSpotAngle, allLights[light].direction, dirVertexToLight);

				sumDiffuse += attenuation * calculateDiffuse(allLights[light].color, lightIntensity, -dirVertexToLight, psIn.normal);
				sumSpecular += attenuation * calculateSpecularHighlight(
					-dirVertexToLight, psIn.normal,
					specularHighlightColor, specularIntensity, specularPower,
					psIn.eyePosition.xyz, psIn.positionPS
				);
				break;
		}
	}

	sumDiffuse = saturate(sumDiffuse);
	sumSpecular = saturate(sumSpecular);

	// Calculate shadow map texture UV coordinates.
	float2 shadowMapCoords;
	shadowMapCoords.x = psIn.shadowMapPosition[0].x / psIn.shadowMapPosition[0].w * 0.5 + 0.5;
	shadowMapCoords.y = -psIn.shadowMapPosition[0].y / psIn.shadowMapPosition[0].w * 0.5 + 0.5;

	float finalDepth = (psIn.shadowMapPosition[0].z / psIn.shadowMapPosition[0].w) - 0.001f;

	// Process shadow maps.
	/*for (unsigned int sc = 0; sc < MAX_SHADOW_CASTER_COUNT; sc++) {
		float sampledDepth = ShadowMapTexture[sc].Sample(ShadowMapSampler[sc], psIn.shadowMapCoord[sc]).r;

		if (psIn.finalDepth[sc] > sampledDepth) {
			sumDiffuse = sumDiffuse * 0.4f;
		}
	}*/
	if (finalDepth > ShadowMapTexture[0].Sample(ShadowMapSampler[0], shadowMapCoords).r) {
		sumDiffuse = sumDiffuse * 0.4f;
	}

	// Use solid color of entity or texture?
	float4 texture_or_solid = entityColor;
	if (useTexture) {
		texture_or_solid = Texture.Sample(Sampler, psIn.texture_UV);
	}

	// Add ambient light & blend the color of the entity.
	psOut.color = float4(
		(sumDiffuse + ambient) * (texture_or_solid + sumSpecular)
	);

	return psOut;
}