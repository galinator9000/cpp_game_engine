#define MAX_JOINT_COUNT 256
#define MAX_JOINT_PER_VERTEX 4

// World matrix is provided by the entity we are currently processing.
cbuffer EntityVSConstantBuffer : register(b0) {
	matrix worldMatrix;
	bool useMeshDeformer;
	float3 padding0;
};

// View and Projection matrices provided by active camera object.
cbuffer CameraVSConstantBuffer : register(b1) {
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 cameraPosition;
	float padding1;
};

// Joint matrices provided by mesh deformer,
// if any deformer attached to current processed entity's mesh.
cbuffer MeshDeformerVSConstantBuffer : register(b2) {
	matrix jointsTransformMatrix[MAX_JOINT_COUNT];
};

// Input structure of the Vertex shader.
struct VSIn {
	float3 position : Position;
	float2 texture_UV : TextureUV;
	float3 normal : Normal;
	float3 tangent : Tangent;
	float3 binormal : Binormal;

	// Per-Vertex joint information, maximum 4 joint supported.
	float4 jointWeights : JointWeights;
	int4 jointIDs : JointIDs;
};

// Output structure of the Vertex shader.
struct VSOut {
	float4 position : SV_Position;
};

// Constant identity matrix.
static const matrix identityMatrix = {
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
};

VSOut main(VSIn vsIn) {
	VSOut vsOut;

	//// Apply joint transforms.
	// Check if current entity uses any mesh deformer.
	float4 finalWorldPosition;
	if (useMeshDeformer) {
		finalWorldPosition = float4(0, 0, 0, 0);

		for (int j = 0; j < MAX_JOINT_PER_VERTEX; j++) {
			int jointId = vsIn.jointIDs[j];
			float jointWeight = vsIn.jointWeights[j];

			if (jointId == -1 || jointWeight < 0.0f) {
				// This and rest of the joints aren't used, break the loop.
				break;
			}

			// Calculate position and normals using joint deform matrices and weight values.
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

	return vsOut;
}