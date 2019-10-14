#include "Lighting.hlsl"

// Provided by entity object.
Texture2D Texture : register(t0);
Texture2D NormalMappingTexture : register(t1);
SamplerState Sampler : register(s0);
cbuffer EntityPSConstantBuffer : register(b0) {
	float3 entityColor;
	bool entityUseTexture;
	bool entityUseNormalMap;
	float4 specularHighlight;
};

cbuffer LightConstantBuffer : register(b1) {
	Light allLights[MAX_LIGHT_COUNT];
};

// Input structure of the Pixel shader.
struct PSIn {
	float3 positionPS : Position;
	float2 texture_UV : TextureUV;

	float3 normal : Normal;
	float3 tangent : Tangent;
	float3 binormal : Binormal;

	float3 eyePosition : EyePosition;
	matrix viewMatrix : ViewMatrix;
};

// Output structure of the Pixel shader.
struct PSOut {
	float4 color : SV_Target;
};

PSOut main(PSIn psIn){
	PSOut psOut;

	float3 normalizedNormal;
	float3 normalizedTangent;
	float3 normalizedBinormal;

	// Normalizing normal input vectors because Rasterizer stage interpolates them.
	normalizedNormal = normalize(psIn.normal);
	normalizedTangent = normalize(psIn.tangent);
	normalizedBinormal = normalize(psIn.binormal);

	// Sample from normal map texture if entity uses it.
	if (entityUseNormalMap) {
		//normalizedNormal = normalize(NormalMappingTexture.Sample(Sampler, psIn.texture_UV).xyz);
	}

	float4 sumDiffuse = float4(0, 0, 0, 0);
	float4 sumSpecular = float4(0, 0, 0, 0);

	float specularPower = specularHighlight.x;
	float specularIntensity = specularHighlight.y;

	//// Calculate all lights.
	for (unsigned int light = 0; light < MAX_LIGHT_COUNT; light++) {
		// Undefined lights' intensity is set to -1.
		if (allLights[light].intensity <= 0.0f) {
			continue;
		}

		float lightIntensity = allLights[light].intensity;

		// Vertex and Light vector calculations.
		float3 vertexToLight = float3(0, 0, 0);
		float distVertexToLight = 0;
		float3 dirVertexToLight = float3(0, 0, 0);

		float attenuation;
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
				sumDiffuse += calculateDiffuse(allLights[light].color, lightIntensity, allLights[light].direction, normalizedNormal);
				break;
			// Point Light calculation.
			case POINT_LIGHT:
				sumDiffuse += attenuation * calculateDiffuse(allLights[light].color, lightIntensity, -dirVertexToLight, normalizedNormal);
				sumSpecular += calculateSpecularHighlight(
					allLights[light].color, lightIntensity, -dirVertexToLight,
					normalizedNormal, attenuation,
					specularIntensity, specularPower,
					psIn.eyePosition.xyz, psIn.positionPS
				);
				break;

			// Spot Light calculation.
			case SPOT_LIGHT:
				lightIntensity *= calculateConeCenterDistance(allLights[light].halfSpotAngle, allLights[light].direction, dirVertexToLight);

				sumDiffuse += attenuation * calculateDiffuse(allLights[light].color, lightIntensity, -dirVertexToLight, normalizedNormal);
				sumSpecular += calculateSpecularHighlight(
					allLights[light].color, lightIntensity, -dirVertexToLight,
					normalizedNormal, attenuation,
					specularIntensity, specularPower,
					psIn.eyePosition.xyz, psIn.positionPS
				);
				break;
		}
	}

	sumDiffuse = saturate(sumDiffuse);
	sumSpecular = saturate(sumSpecular);

	// Use solid color of entity or texture?
	float4 texture_or_solid = float4(entityColor, 1.0f);
	if (entityUseTexture) {
		texture_or_solid = Texture.Sample(Sampler, psIn.texture_UV);
	}

	//// Add ambient light & blend the color of the entity.
	psOut.color = float4(
		texture_or_solid + (ambient + sumDiffuse + sumSpecular)
	);

	return psOut;
}