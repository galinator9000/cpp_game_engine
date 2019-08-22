// World matrix is provided by the entity we are currently processing.
cbuffer EntityConstantBuffer : register(b0) {
	matrix worldMatrix;
};

// View and Projection matrices provided by active camera object.
cbuffer ViewProjectionMatrices : register(b1) {
	matrix viewMatrix;
	matrix projectionMatrix;
};

// Light buffers.
// Directional Light direction and intensity values.
/*cbuffer DirectionalLightConstantBuffer : register(b2) {
	float3 lightDirection;
	float lightIntensity;
}; */

// Point Light position and intensity values.
cbuffer PointLightConstantBuffer : register(b2) {
	float3 lightPosition;
	float lightIntensity;
};
// Attenuation calculation values for Point Light.
static const float attenuation_constant = 1.0f;
static const float attenuation_linear = 0.014;
static const float attenuation_quadratic = 0.0007;

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
		float4(vsIn.position, 1.0f), worldMatrix
	);
	finalVector = mul(finalVector, viewMatrix);
	finalVector = mul(finalVector, projectionMatrix);
	vsOut.position = finalVector;

	// Rotate the normals.
	float3 rotatedNormal = mul(vsIn.normal, (float3x3) worldMatrix);

	//// Directional Light calculation.
	//float4 diffuse = lightIntensity * max(0.0f, dot(-lightDirection, rotatedNormal));

	//// Point Light calculation.
	float3 vertexToLight = (float3) float4(lightPosition, 1.0f) - mul(float4(vsIn.position, 1.0f), worldMatrix);
	float distVertexToLight = length(vertexToLight);
	float3 directionVertexToLight = vertexToLight / distVertexToLight;

	// Attenuation calculation.
	float attenuation = attenuation_constant + (attenuation_linear * distVertexToLight) + (attenuation_quadratic * (distVertexToLight * distVertexToLight));
	float4 diffuse = lightIntensity * attenuation * max(0.0f, dot(directionVertexToLight, rotatedNormal));

	// Add ambient light & blend the color of the entity.
	vsOut.color = saturate(
		(diffuse + ambient) * vsIn.color
	);
	vsOut.color.a = 1.0f;

	return vsOut;
}