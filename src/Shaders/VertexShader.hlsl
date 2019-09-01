#define MAX_JOINT_COUNT 64

// World matrix is provided by the entity we are currently processing.
cbuffer EntityVSConstantBuffer : register(b0) {
	matrix worldMatrix;
};

// View and Projection matrices provided by active camera object.
cbuffer CameraVSConstantBuffer : register(b1) {
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 cameraPosition;
};

// Joint matrices provided by mesh deformer,
// if any deformer attached to current processed entity's mesh.
cbuffer MeshDeformerVSConstantBuffer : register(b2) {
	matrix jointsTransformMatrix[MAX_JOINT_COUNT];
};

// Input structure of the Vertex shader.
struct VSIn {
	float3 position : Position;
	float3 normal : Normal;
	float2 texture_UV : TextureUV;
};

// Output structure of the Vertex shader.
struct VSOut {
	// These values will be passed to pixel shader.
	float3 positionPS : Position;
	float3 normal : Normal;
	float2 texture_UV : TextureUV;
	float4 eyePosition : EyePosition;
	matrix viewMatrix : ViewMatrix;

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
	vsOut.positionPS = mul(float4(vsIn.position, 1.0f), worldMatrix).xyz;
	// Rotate the normals.
	// When this normal vector passed to pixel shader, it will be interpolated by rasterizer.
	vsOut.normal = mul(vsIn.normal, (float3x3) worldMatrix);
	vsOut.texture_UV = vsIn.texture_UV;
	vsOut.eyePosition = cameraPosition;
	vsOut.viewMatrix = viewMatrix;

	return vsOut;
}