#define MAX_JOINT_COUNT 64
#define MAX_JOINT_PER_VERTEX 4

// World matrix is provided by the entity we are currently processing.
cbuffer EntityVSConstantBuffer : register(b0) {
	matrix worldMatrix;
	bool useMeshDeformer;
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

	// Per-Vertex joint information, maximum 4 joint supported.
	float4 jointWeights : JointWeights;
	int4 jointIDs : JointIDs;
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

// Constant identity matrix.
static const matrix identityMatrix = {
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
};

VSOut main(VSIn vsIn){
	VSOut vsOut;

	//// Apply joint transforms.
	// Check if current entity uses any mesh deformer.
	float4 finalWorldPosition;
	if (useMeshDeformer) {
		finalWorldPosition = float4(0, 0, 0, 0);

		// Only apply first joint.
		// For debugging.
		//finalWorldPosition += mul(float4(vsIn.position, 1.0f), jointsTransformMatrix[vsIn.jointIDs[0]]) * 1.0f;

		for (int j = 0; j < MAX_JOINT_PER_VERTEX; j++) {
			int jointId = vsIn.jointIDs[j];
			float jointWeight = vsIn.jointWeights[j];

			if (jointId == -1 || jointWeight < 0.0f) {
				// This and rest of the joints aren't used, break the loop.
				break;
			}

			finalWorldPosition += mul(float4(vsIn.position, 1.0f), jointsTransformMatrix[jointId]) * jointWeight;
		}
	}
	// If mesh deformer isn't used, just take incoming vertex position.
	else {
		finalWorldPosition = float4(vsIn.position, 1.0f);
	}

	//// Apply "Model, View, Projection" transform matrices.
	// Apply "Model" matrix.
	finalWorldPosition = mul(finalWorldPosition, worldMatrix);

	// Apply "View" and "Projection" transform matrices.
	vsOut.position = mul(finalWorldPosition, viewMatrix);
	vsOut.position = mul(vsOut.position, projectionMatrix);

	// Pixel shader needs just world positions without Projection and View.
	vsOut.positionPS = finalWorldPosition.xyz;

	// Rotate the normals with joint and world matrices.
	// When this normal vector passed to pixel shader, it will be interpolated by rasterizer.
	float3 normal = vsIn.normal;
	//normal = mul(normal, (float3x3) weightedJointTransforms);
	normal = mul(normal, (float3x3) worldMatrix);
	vsOut.normal = normal;

	vsOut.texture_UV = vsIn.texture_UV;
	vsOut.eyePosition = cameraPosition;
	vsOut.viewMatrix = viewMatrix;

	return vsOut;
}