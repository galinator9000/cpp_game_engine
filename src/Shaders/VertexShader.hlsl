// World matrix is provided by the entity we are currently processing.
cbuffer EntityConstantBuffer : register(b0) {
	matrix worldMatrix;
};

// View and Projection matrices provided by active camera object.
cbuffer ViewProjectionMatrices : register(b1) {
	matrix viewMatrix;
	matrix projectionMatrix;
};

// Light intensity, direction and position values.
cbuffer LightConstantBuffer : register(b2) {
	float lightIntensity;
	float3 lightDirection;
	float3 lightPosition;
	unsigned int lightType;
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

// Constant values
// Current processed light type.
static const unsigned int DIRECTIONAL_LIGHT = 0;
static const unsigned int POINT_LIGHT = 1;

// Ambient value for light calculation.
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

	//// Light calculation per-vertex.
	// Rotate the normals.
	float3 rotatedNormal = mul(vsIn.normal, (float3x3) worldMatrix);

	float4 diffuse = float4(0, 0, 0, 0);
	// Directional Light calculation.
	if (lightType == DIRECTIONAL_LIGHT) {
		diffuse = lightIntensity * max(0.0f, dot(-lightDirection, rotatedNormal));
	}
	// Point Light calculation.
	else if (lightType == POINT_LIGHT) {
		float3 vertexToLight = (float3) float4(lightPosition, 1.0f) - mul(float4(vsIn.position, 1.0f), worldMatrix);
		float distVertexToLight = length(vertexToLight);
		float3 directionVertexToLight = normalize(vertexToLight);

		// Attenuation calculation.
		float attenuation = attenuation_constant + (attenuation_linear * distVertexToLight) + (attenuation_quadratic * (distVertexToLight * distVertexToLight));
		diffuse = lightIntensity * attenuation * max(0.0f, dot(directionVertexToLight, rotatedNormal));
	}

	//// Add ambient light & blend the color of the entity.
	vsOut.color = saturate(
		(diffuse + ambient) * vsIn.color
	);
	vsOut.color.a = 1.0f;

	return vsOut;
}