// Light direction and intensity values.
cbuffer LightConstantBuffer : register(b0) {
	float3 direction;
	float intensity;
};

// Input structure of the Pixel shader.
struct PSIn {
	float3 normal : Normal;
	float4 color : Color;
};

// Output structure of the Pixel shader.
struct PSOut {
	float4 color : SV_Target;
};

PSOut main(PSIn psIn){
	PSOut psOut;

	// Light calculation.
	float4 diffuse = intensity * max(0.0f, dot(-direction, psIn.normal));

	// Add ambient light & blend the color of the entity.
	psOut.color = saturate(
		(diffuse + float4(0.15f, 0.15f, 0.15f, 0.15f)) * psIn.color
	);

	return psOut;
}