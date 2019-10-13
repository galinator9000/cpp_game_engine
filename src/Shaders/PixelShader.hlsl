#define MAX_LIGHT_COUNT 8

// Light type enums.
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

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

//// Light calculation.
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

cbuffer LightConstantBuffer : register(b1) {
	Light allLights[MAX_LIGHT_COUNT];
};

// Constant values
// Attenuation calculation values for Point Light.
static const float attenuation_constant = 1.0f;
static const float attenuation_linear = 0.014;
static const float attenuation_quadratic = 0.0007;

// Ambient value for light calculation.
static const float4 ambient = float4(0.03f, 0.03f, 0.03f, 0);

// Input structure of the Pixel shader.
struct PSIn {
	float3 positionPS : Position;
	float2 texture_UV : TextureUV;

	float3 normal : Normal;
	float3 tangent : Tangent;
	float3 binormal : Binormal;

	float4 eyePosition : EyePosition;
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
		/*normalizedNormal = normalize(NormalMappingTexture.Sample(Sampler, psIn.texture_UV).xyz);
		normalizedNormal.x = (normalizedNormal.x * 2) - 1.0f;
		normalizedNormal.y = (normalizedNormal.y * 2) - 1.0f;
		normalizedNormal.z = normalizedNormal.z;*/
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

		float4 diffuse = float4(0, 0, 0, 0);
		float4 specular = float4(0, 0, 0, 0);
		float lightIntensity = allLights[light].intensity;

		float3 vertexToLight = float3(0, 0, 0);
		float distVertexToLight = 0;
		float3 directionVertexToLight = float3(0, 0, 0);
		float attenuation = 0;
		float3 reflectionVector = float3(0, 0, 0);
		
		switch (allLights[light].type) {
			// Directional Light calculation.
			case DIRECTIONAL_LIGHT:
				diffuse = allLights[light].color * lightIntensity * max(0.0f, dot(-allLights[light].direction, normalizedNormal));
				break;

			// Point Light calculation.
			case POINT_LIGHT:
				// Vertex and Light vector calculations.
				vertexToLight = allLights[light].position - psIn.positionPS;
				distVertexToLight = length(vertexToLight);
				directionVertexToLight = normalize(vertexToLight);

				// Attenuation calculation.
				attenuation = 1.0f / (attenuation_constant + (attenuation_linear * distVertexToLight) + (attenuation_quadratic * (distVertexToLight * distVertexToLight)));
				diffuse = allLights[light].color * lightIntensity * attenuation * max(0.0f, dot(directionVertexToLight, normalizedNormal));

				// Specular highlight calculation.
				reflectionVector = reflect(
					-vertexToLight,
					normalizedNormal
				);
				specular = allLights[light].color * lightIntensity * attenuation * specularIntensity * pow(
					max(
						0.0f,
						dot(
							normalize(reflectionVector),
							normalize(psIn.eyePosition.xyz - psIn.positionPS)
						)
					),
					specularPower
				);
				break;

			// Spot Light calculation.
			case SPOT_LIGHT:
				// Vertex and Light vector calculations.
				vertexToLight = allLights[light].position - psIn.positionPS;
				distVertexToLight = length(vertexToLight);
				directionVertexToLight = normalize(vertexToLight);

				// Spot Light calculation.
				// Cone calculation of the Spot light.
				float minCos = cos(allLights[light].halfSpotAngle);
				float maxCos = (minCos + 1.0f) / 2.0f;
				float cosAngle = dot(allLights[light].direction, -directionVertexToLight);
				lightIntensity = smoothstep(minCos, maxCos, cosAngle);

				// Attenuation calculation.
				attenuation = 1.0f / (attenuation_constant + (attenuation_linear * distVertexToLight) + (attenuation_quadratic * (distVertexToLight * distVertexToLight)));
				diffuse = allLights[light].color * lightIntensity * attenuation * max(0.0f, dot(directionVertexToLight, normalizedNormal));

				// Specular highlight calculation.
				reflectionVector = reflect(
					-vertexToLight,
					normalizedNormal
				);
				specular = allLights[light].color * lightIntensity * attenuation * specularIntensity * pow(
					max(
						0.0f,
						dot(
							normalize(reflectionVector),
							normalize(psIn.eyePosition.xyz - psIn.positionPS)
						)
					),
					specularPower
				);
				break;
		}

		sumDiffuse += diffuse;
		sumSpecular += specular;
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