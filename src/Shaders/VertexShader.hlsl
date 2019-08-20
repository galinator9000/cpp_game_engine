// World matrix is provided by the entity we are currently processing.
cbuffer ConstantBuffer : register(b0) {
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
    float4 color : Color;
	float4 pos : SV_Position;
};

VSOut main(VSIn vertex){
	VSOut vso;

	// Final position vector
	float4 finalVector;
	finalVector = mul(
		float4(vertex.position.x, vertex.position.y, vertex.position.z, 1.0f), worldMatrix
	);
	finalVector = mul(finalVector, viewMatrix);
	finalVector = mul(finalVector, projectionMatrix);
	vso.pos = finalVector;

	// Color
	vso.color = vertex.color;

	return vso;
}