#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1

Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

//// Light calculation.
// Light intensity, direction and position values.
cbuffer LightConstantBuffer : register(b0) {
	float lightIntensity;
	float3 lightDirection;
	float3 lightPosition;
	unsigned int lightType;
};

// Constant values
// Attenuation calculation values for Point Light.
static const float attenuation_constant = 1.0f;
static const float attenuation_linear = 0.014;
static const float attenuation_quadratic = 0.0007;

// Ambient value for light calculation.
static const float4 ambient = float4(0.10f, 0.10f, 0.10f, 0);

// Input structure of the Pixel shader.
struct PSIn {
	float4 positionPS : Position;
	float3 normal : Normal;
	float4 color : Color;
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

	float4 diffuse = float4(0, 0, 0, 0);
	// Directional Light calculation.
	if (lightType == DIRECTIONAL_LIGHT) {
		diffuse = lightIntensity * max(0.0f, dot(-lightDirection, normalizedNormal));
	}
	// Point Light calculation.
	else if (lightType == POINT_LIGHT) {
		float3 vertexToLight = (float3) float4(lightPosition, 1.0f) - psIn.positionPS;
		float distVertexToLight = length(vertexToLight);
		float3 directionVertexToLight = normalize(vertexToLight);

		// Attenuation calculation.
		float attenuation = attenuation_constant + (attenuation_linear * distVertexToLight) + (attenuation_quadratic * (distVertexToLight * distVertexToLight));
		diffuse = lightIntensity * attenuation * max(0.0f, dot(directionVertexToLight, normalizedNormal));
	}

	float4 texture_or_solid;

	// If sampled texture color is pure black, use entity color..
	// This is actually stupid if your texture has pure black color. (0,0,0)
	texture_or_solid = Texture.Sample(Sampler, psIn.texture_UV);
	if (!all(texture_or_solid)) {
		texture_or_solid = psIn.color;
	}

	//// Add ambient light & blend the color of the entity.
	psOut.color = saturate(
		(diffuse + ambient) * texture_or_solid
	);
	psOut.color.a = 1.0f;

	return psOut;
}