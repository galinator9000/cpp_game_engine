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
cbuffer LightConstantBuffer : register(b2){
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

    // Directional light calculation
    float diffuse = intensity * dot(clamp(vertex.normal, 0.0f, 1.0f), -direction);

    // Blend color and light.
    vso.color = vertex.color * diffuse;

    // Add ambient light.
    vso.color += float4(0.15f, 0.15f, 0.15f, 1.0f);

    // Clamp values.
    vso.color = clamp(vso.color, 0.0f, 1.0f);

	return vso;
}