#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define MAX_LIGHT_COUNT 16

// Provided by entity object.
Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);
cbuffer EntityPSConstantBuffer : register(b0) {
	float3 entityColor;
	bool entityUseTexture;
	float4 specularHighlight;
};

//// Light calculation.
// Light intensity, direction and position values.
struct Light {
	float4 color;
	float intensity;
	float3 direction;
	float3 position;
	unsigned int type;
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
	float3 normal : Normal;
	float2 texture_UV : TextureUV;
	float4 eyePosition : EyePosition;
	matrix viewMatrix : ViewMatrix;
};

// Output structure of the Pixel shader.
struct PSOut {
	float4 color : SV_Target;
};

PSOut main(PSIn psIn){
	PSOut psOut;

	// Normalizing normal input vector because Rasterizer stage interpolates it.
	float3 normalizedNormal = normalize(psIn.normal);

	float4 sumSpecular = float4(0, 0, 0, 0);
	float4 sumDiffuse = float4(0, 0, 0, 0);

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
		
		switch (allLights[light].type) {
			// Directional Light calculation.
			case DIRECTIONAL_LIGHT:
				diffuse = allLights[light].color * allLights[light].intensity * max(0.0f, dot(-allLights[light].direction, normalizedNormal));
				break;

			// Point Light calculation.
			case POINT_LIGHT:
				// Vertex and Light vector calculations.
				float3 vertexToLight = allLights[light].position - psIn.positionPS;
				float distVertexToLight = length(vertexToLight);
				float3 directionVertexToLight = normalize(vertexToLight);

				// Attenuation calculation.
				float attenuation = 1.0f / (attenuation_constant + (attenuation_linear * distVertexToLight) + (attenuation_quadratic * (distVertexToLight * distVertexToLight)));
				diffuse = allLights[light].color * allLights[light].intensity * attenuation * max(0.0f, dot(directionVertexToLight, normalizedNormal));

				// Specular highlight calculation.
				float3 reflectionVector = reflect(
					-vertexToLight,
					normalizedNormal
				);
				specular = allLights[light].intensity * attenuation * specularIntensity * pow(
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

	// Use solid color of entity or texture?
	float4 texture_or_solid;
	if (entityUseTexture) {
		texture_or_solid = Texture.Sample(Sampler, psIn.texture_UV);
	}else {
		texture_or_solid = float4(entityColor, 1.0f);
	}

	//// Add ambient light & blend the color of the entity.
	psOut.color = float4(
		saturate(
			texture_or_solid + ambient + (sumDiffuse + sumSpecular)
		)
	);

	return psOut;
}