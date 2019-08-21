// World matrix is provided by the entity we are currently processing.
cbuffer EntityConstantBuffer : register(b0) {
	matrix worldMatrix;
};

// View and Projection matrices provided by active camera object.
cbuffer ViewProjectionMatrices : register(b1) {
	matrix viewMatrix;
	matrix projectionMatrix;
};

// Light direction and intensity values.
cbuffer LightConstantBuffer : register(b2) {
	float3 direction;
	float intensity;
};

// Input structure of the Vertex shader.
struct VSIn {
	float3 position : Position;
	float3 normal : Normal;
	float4 color : Color;
};

// Output structure of the Vertex shader.
struct VSOut {
	// These values will be passed to pixel shader.
	float4 color : Color;

	float4 position : SV_Position;
};

// Constant vectors
static const float4 ambient = float4(0.10f, 0.10f, 0.10f, 0);

VSOut main(VSIn vsIn){
	VSOut vsOut;

	// Position vector
	float4 finalVector;
	finalVector = mul(
		float4(vsIn.position.x, vsIn.position.y, vsIn.position.z, 1.0f), worldMatrix
	);
	finalVector = mul(finalVector, viewMatrix);
	finalVector = mul(finalVector, projectionMatrix);
	vsOut.position = finalVector;

	// Light calculation.
	float3 rotatedNormal = mul(vsIn.normal, (float3x3) worldMatrix);

	float4 diffuse = intensity * max(0.0f, dot(-direction, rotatedNormal));

	// Add ambient light & blend the color of the entity.
	vsOut.color = saturate(
		(diffuse) * vsIn.color
	);
	vsOut.color.a = 1.0f;

	return vsOut;
}