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
	float2 texture_UV : TextureUV;
};

// Output structure of the Vertex shader.
struct VSOut {
	// These values will be passed to pixel shader.
	float4 positionPS : Position;
	float3 normal : Normal;
	float4 color : Color;
	float2 texture_UV : TextureUV;

	float4 position : SV_Position;
};

VSOut main(VSIn vsIn){
	VSOut vsOut;

	//// Position vector
	float4 finalVector;
	finalVector = mul(
		float4(vsIn.position, 1.0f), worldMatrix
	);
	finalVector = mul(finalVector, viewMatrix);
	finalVector = mul(finalVector, projectionMatrix);
	vsOut.position = finalVector;

	//// These values will be passed to pixel shader.
	vsOut.positionPS = mul(float4(vsIn.position, 1.0f), worldMatrix);
	// Rotate the normals.
	// When this normal vector passed to pixel shader, it will be interpolated by rasterizer.
	vsOut.normal = mul(vsIn.normal, (float3x3) worldMatrix);
	vsOut.color = vsIn.color;
	vsOut.texture_UV = vsIn.texture_UV;

	return vsOut;
}