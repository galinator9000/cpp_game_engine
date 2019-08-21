// World matrix is provided by the entity we are currently processing.
cbuffer EntityConstantBuffer : register(b0) {
	matrix worldMatrix;
};

// View and Projection matrices provided by active camera object.
cbuffer ViewProjectionMatrices : register(b1) {
	matrix viewMatrix;
	matrix projectionMatrix;
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
	float3 normal : Normal;
	float4 color : Color;

	float4 position : SV_Position;
};

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

	// Fill values that will be passed to pixel shader.
	vsOut.normal = mul(vsIn.normal, (float3x3) worldMatrix);
	vsOut.color = vsIn.color;

	return vsOut;
}