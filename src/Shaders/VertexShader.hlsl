// Output structure of the Vertex shader.
struct VertexIn {
	float3 position : Position;
	float3 normal : Normal;
};

// Output structure of the Vertex shader.
struct VSOut {
	float4 pos : SV_Position;
};

// World matrix is provided by the entity we are currently processing.
cbuffer ConstantBuffer : register(b0) {
	matrix worldMatrix;
};

// View and Projection matrices provided by active camera object.
cbuffer ViewProjectionMatrices : register(b1) {
	matrix viewMatrix;
	matrix projectionMatrix;
};

VSOut main(VertexIn vertex){
	VSOut vso;

	float4 finalVector;
	finalVector = mul(
		float4(vertex.position.x, vertex.position.y, vertex.position.z, 1.0f), worldMatrix
	);
	finalVector = mul(finalVector, viewMatrix);
	finalVector = mul(finalVector, projectionMatrix);

	vso.pos = finalVector;
	return vso;
}