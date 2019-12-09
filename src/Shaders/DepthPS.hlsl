#include "Lighting.hlsl"

// Provided by entity object.
Texture2D AlphaTexture : register(t3);

// Various samplers.
SamplerState defaultSampler : register(s0);

cbuffer MaterialSConstantBuffer : register(b0) {
	float4 materialColor;
	float specularPower;
	float specularIntensity;
	float2 padding;
	bool useDiffuse;
	bool useNormalMapping;
	bool useAlpha;
	bool useSpecular;
};

// Input structure of the Pixel shader.
struct PSIn {
	// Vertex values
	float2 texture_UV : TextureUV;
};

float4 main(PSIn psIn) : SV_Target{
	// If uses alpha value from texture, sample it.
	if (useAlpha) {
		float3 alpha = AlphaTexture.Sample(defaultSampler, psIn.texture_UV).rgb;
		if (((alpha.r + alpha.g + alpha.b) / 3) < 0.3f) {
			discard;
		}
	}

	return float4(1, 1, 1, 1);
}