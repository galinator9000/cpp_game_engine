#include "Structs.hlsli"
#include "Buffers.hlsli"

SamplerState whiteBorderSampler : register(s2);
Texture2D ShadowMapTexture[MAX_SHADOWBOX_COUNT * MAX_SHADOWMAP_COUNT] : register(t3);

// Lights constant buffer.
cbuffer LightConstantBuffer : register(b1) {
	Light allLights[MAX_LIGHT_COUNT];
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
	in float4 shadowMapPosition[MAX_SHADOWBOX_COUNT * MAX_SHADOWMAP_COUNT],
	in unsigned int shadowMapIndices[MAX_SHADOWBOX_COUNT],
	float3 position,
	float3 eyePosition
) {
	float2 shadowMapCoords;
	float finalDepth;
	float nearestObjectDepth;

	float shadowDistance;
	float transitionDistance;
	float distanceVal;
	float fadingFactor = 1;

	// Process shadow maps.
	float shadowFactor = 0;

	unsigned int shadowMapIndex;
	bool secondaryCondition = true;
	[unroll]
	for (unsigned int sb = 0; sb < MAX_SHADOWBOX_COUNT; sb++) {
		[unroll]
		for (unsigned int sm = 0; sm < MAX_SHADOWMAP_COUNT; sm++) {
			float3 vertexToLight;
			float3 dirVertexToLight;
			float distVertexToLight;

			switch (shadowBoxes[sb].lightType) {
				case DIRECTIONAL_LIGHT:
					secondaryCondition = (sm == shadowMapIndices[sb]);
					break;
				case POINT_LIGHT:
					secondaryCondition = true;
					break;
				case SPOT_LIGHT:
					secondaryCondition = true;
					break;
			}

			if (shadowBoxes[sb].isActive && secondaryCondition) {
				// Distance calculations for smooth shadow transition.
				switch (shadowBoxes[sb].lightType) {
					case DIRECTIONAL_LIGHT:
						shadowDistance = shadowBoxes[sb].shadowDistance;
						transitionDistance = shadowBoxes[sb].shadowDistance * 0.03;
						distanceVal = distance(eyePosition, position);
						distanceVal = distanceVal - (shadowDistance - transitionDistance);
						distanceVal = distanceVal / transitionDistance;
						fadingFactor = clamp(1.0 - distanceVal, 0, 1);
						break;

					case POINT_LIGHT:
						vertexToLight = allLights[shadowBoxes[sb].lightID].position - position;
						dirVertexToLight = normalize(vertexToLight);
						distVertexToLight = length(vertexToLight);
						fadingFactor = calculateAttenuation(distVertexToLight);
						break;

					case SPOT_LIGHT:
						vertexToLight = allLights[shadowBoxes[sb].lightID].position - position;
						dirVertexToLight = normalize(vertexToLight);
						distVertexToLight = length(vertexToLight);
						fadingFactor = calculateAttenuation(distVertexToLight) * calculateConeCenterDistance(
							allLights[shadowBoxes[sb].lightID].halfSpotAngle,
							allLights[shadowBoxes[sb].lightID].direction,
							dirVertexToLight
						);
						break;
				}

				shadowMapIndex = sb * MAX_SHADOWMAP_COUNT + sm;
				shadowMapCoords.x = shadowMapPosition[shadowMapIndex].x / shadowMapPosition[shadowMapIndex].w * 0.5 + 0.5;
				shadowMapCoords.y = -shadowMapPosition[shadowMapIndex].y / shadowMapPosition[shadowMapIndex].w * 0.5 + 0.5;

				if ((saturate(shadowMapCoords.x) == shadowMapCoords.x) && (saturate(shadowMapCoords.y) == shadowMapCoords.y)) {
					finalDepth = (shadowMapPosition[shadowMapIndex].z / shadowMapPosition[shadowMapIndex].w) - 0.0001f;
					finalDepth = clamp(finalDepth, 0, 1);

					nearestObjectDepth = ShadowMapTexture[shadowMapIndex].Sample(whiteBorderSampler, shadowMapCoords).r;

					if (finalDepth > nearestObjectDepth && distance(finalDepth, nearestObjectDepth) > 0.001f) {
						shadowFactor = 0.6 * fadingFactor;

						// Shadow each pixel only once.
						break;
					}
				}
			}
		}
	}

	shadowFactor = clamp(shadowFactor, 0, 1);
	return shadowFactor;
}