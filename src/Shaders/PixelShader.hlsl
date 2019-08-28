#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define MAX_LIGHT_COUNT 8

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
static const float3 ambient = float3(0.10f, 0.10f, 0.10f);

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

	// Normalize and rotate the normals.
	// Normalizing normal input vector because Rasterizer stage interpolates it.
	float3 normalizedNormal = normalize(psIn.normal);

	float3 sumSpecular = float3(0, 0, 0);
	float3 sumDiffuse = float3(0, 0, 0);

	//// Calculate all lights.
	for (unsigned int light = 0; light < MAX_LIGHT_COUNT; light++) {
		// Undefined lights' intensity is set to -1.
		if (allLights[light].intensity < 0.0f) {
			continue;
		}

		float3 diffuse = float3(0, 0, 0);
		float3 specular = float3(0,0,0);

		// Vertex and Light vector calculations.
		float3 vertexToLight;
		switch (allLights[light].type) {
			case DIRECTIONAL_LIGHT:
				vertexToLight = -allLights[light].direction;
				break;
			case POINT_LIGHT:
				vertexToLight = allLights[light].position - psIn.positionPS;
				break;
		}
		float distVertexToLight = length(vertexToLight);
		float3 directionVertexToLight = normalize(vertexToLight);

		// Specular highlight calculation.
		float specularPower = specularHighlight.x;
		float specularIntensity = specularHighlight.y;

		float3 reflectionVector = reflect(
			-vertexToLight,
			normalizedNormal
		);

		specular = allLights[light].intensity * specularIntensity * pow(
			max(
				0.0f,
				dot(
					normalize(reflectionVector),
					normalize(psIn.eyePosition.xyz - psIn.positionPS)
				)
			),
			specularPower
		);
		
		switch (allLights[light].type) {
			// Directional Light calculation.
			case DIRECTIONAL_LIGHT:
				diffuse = allLights[light].intensity * max(0.0f, dot(-allLights[light].direction, normalizedNormal));
				break;

			// Point Light calculation.
			case POINT_LIGHT:
				// Attenuation calculation.
				float attenuation = attenuation_constant + (attenuation_linear * distVertexToLight) + (attenuation_quadratic * (distVertexToLight * distVertexToLight));
				specular = specular * attenuation;

				diffuse = allLights[light].intensity * attenuation * max(0.0f, dot(directionVertexToLight, normalizedNormal));
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
			(sumDiffuse + ambient + sumSpecular) * texture_or_solid.xyz
		),
		1.0f
	);

	return psOut;
}