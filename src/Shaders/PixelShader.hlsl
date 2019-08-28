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
	float lightIntensity;
	float3 lightDirection;
	float3 lightPosition;
	unsigned int lightType;
};

cbuffer LightConstantBuffer : register(b1) {
	Light lights[MAX_LIGHT_COUNT];
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
	float4 positionPS : Position;
	float3 normal : Normal;
	float2 texture_UV : TextureUV;
};

// Output structure of the Pixel shader.
struct PSOut {
	float4 color : SV_Target;
};

PSOut main(PSIn psIn){
	PSOut psOut;

	//// Light calculation per-vertex.
	// Normalize and rotate the normals.
	// Normalizing normal input vector because Rasterizer stage interpolates it.
	float3 normalizedNormal = normalize(psIn.normal);

	// Vertex and Light vector calculations.
	float3 vertexToLight = lights[0].lightPosition - (float3) psIn.positionPS;
	float distVertexToLight = length(vertexToLight);
	float3 directionVertexToLight = normalize(vertexToLight);

	float3 diffuse = float3(0, 0, 0);
	// Directional Light calculation.
	if (lights[0].lightType == DIRECTIONAL_LIGHT) {
		diffuse = lights[0].lightIntensity * max(0.0f, dot(-lights[0].lightDirection, normalizedNormal));
	}
	// Point Light calculation.
	else if (lights[0].lightType == POINT_LIGHT) {
		// Attenuation calculation.
		float attenuation = attenuation_constant + (attenuation_linear * distVertexToLight) + (attenuation_quadratic * (distVertexToLight * distVertexToLight));
		diffuse = lights[0].lightIntensity * attenuation * max(0.0f, dot(directionVertexToLight, normalizedNormal));
	}

	// Specular highlight calculation.
	float3 specular = float3(0, 0, 0);
	float specularPower = specularHighlight.x;
	float specularIntensity = specularHighlight.y;

	float3 w_vector = normalizedNormal * dot(directionVertexToLight, normalizedNormal);
	float3 reflectionVector = w_vector * 2.0f - directionVertexToLight;
	specular = lights[0].lightIntensity * specularIntensity * pow(
		max(
			0.0f,
			dot(
				normalize(reflectionVector),
				normalize((float3) psIn.positionPS)
			)
		),
		specularPower
	);

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
			(diffuse + ambient + specular) * (float3) texture_or_solid
		),
		1.0f
	);

	return psOut;
}