#include "Structs.hlsli"
#include "Constants.hlsli"

SamplerState whiteBorderSampler : register(s2);
Texture2D ShadowMapTexture[MAX_SHADOW_CASTER_COUNT] : register(t3);

// Lights constant buffer.
cbuffer LightConstantBuffer : register(b1) {
	Light allLights[MAX_LIGHT_COUNT];
};

// Shadow map constant buffers.
cbuffer ShadowMapVSConstantBuffer : register(b2) {
	ShadowMap shadowMaps[MAX_SHADOW_CASTER_COUNT];
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

	float4 finalOutput = (sumDiffuse + sumSpecularHighlight);
	finalOutput.a = 1;

	return finalOutput;
}

float calculateAllShadows(
	in float4 shadowMapPosition[MAX_SHADOW_CASTER_COUNT],
	float3 position,
	float3 eyePosition
) {
	float2 shadowMapCoords;
	float finalDepth;
	float nearestObjectDepth;

	float shadowDistance;
	float transitionDistance;
	float distanceVal;
	float fadingFactor = 0;

	// Process shadow maps.
	float shadowFactor = 0;
	for (unsigned int sc = 0; sc < MAX_SHADOW_CASTER_COUNT; sc++) {
		if (shadowMaps[sc].isActive) {
			// Distance calculations for smooth shadow transition.
			switch (shadowMaps[sc].lightType) {
				case DIRECTIONAL_LIGHT:
					shadowDistance = shadowMaps[sc].shadowDistance;
					transitionDistance = shadowMaps[sc].shadowDistance * 0.03;
					distanceVal = distance(eyePosition, position);
					distanceVal = distanceVal - (shadowDistance - transitionDistance);
					distanceVal = distanceVal / transitionDistance;
					fadingFactor = clamp(1.0 - distanceVal, 0, 1);

					break;
				case SPOT_LIGHT:
					fadingFactor = calculateConeCenterDistance(
						allLights[shadowMaps[sc].lightID].halfSpotAngle,
						allLights[shadowMaps[sc].lightID].direction,
						normalize(allLights[shadowMaps[sc].lightID].position - position)
					);

					break;
			}

			shadowMapCoords.x = shadowMapPosition[sc].x / shadowMapPosition[sc].w * 0.5 + 0.5;
			shadowMapCoords.y = -shadowMapPosition[sc].y / shadowMapPosition[sc].w * 0.5 + 0.5;

			if ((saturate(shadowMapCoords.x) == shadowMapCoords.x) && (saturate(shadowMapCoords.y) == shadowMapCoords.y)) {
				finalDepth = (shadowMapPosition[sc].z / shadowMapPosition[sc].w) - 0.001f;
				if (finalDepth > 1.0f) {
					finalDepth = 0;
				}

				nearestObjectDepth = ShadowMapTexture[sc].Sample(whiteBorderSampler, shadowMapCoords).r;

				if (finalDepth > nearestObjectDepth) {
					shadowFactor = 0.6 * fadingFactor;

					// Shadow each pixel only once.
					break;
				}
			}
		}
	}

	return shadowFactor;
}