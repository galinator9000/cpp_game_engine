#include "Structs.hlsli"
#include "Buffers.hlsli"

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
	// Vertex values
	float3 positionPS : Position;
	float2 texture_UV : TextureUV;
	float3 normal : Normal;
	float3 tangent : Tangent;
	float3 binormal : Binormal;

	// Camera position in world space.
	float3 eyePosition : EyePosition;
	// Specifies which subfrustum to sample from.
	unsigned int subFrustumIndices[MAX_SHADOW_CASTER_COUNT]: SubFrustumIndices;

	// Shadow map
	// XY, shadow map texture UV coordinates
	// Z, distance from light.
	float4 shadowMapPosition[MAX_SHADOW_CASTER_COUNT * MAX_CSM_SUBFRUSTUM_COUNT] : TEXCOORD0;

	// Final vertex shader output
	float4 position : SV_Position;
};

VSOut main(VSIn vsIn){
	VSOut vsOut;

	//// Apply joint transforms.
	// Check if current entity uses any mesh deformer.
	float4 finalWorldPosition;
	float3 finalNormal;
	float3 finalTangent;
	float3 finalBinormal;
	if (useMeshDeformer) {
		finalWorldPosition = float4(0, 0, 0, 0);

		finalNormal = float3(0, 0, 0);
		finalTangent = float3(0, 0, 0);
		finalBinormal = float3(0, 0, 0);

		for (int j = 0; j < MAX_JOINT_PER_VERTEX; j++) {
			int jointId = vsIn.jointIDs[j];
			float jointWeight = vsIn.jointWeights[j];

			if (jointId == -1 || jointWeight < 0.0f) {
				// This and rest of the joints aren't used, break the loop.
				break;
			}

			// Calculate position and normals using joint deform matrices and weight values.
			finalWorldPosition += mul(float4(vsIn.position, 1.0f), jointsTransformMatrix[jointId]) * jointWeight;

			finalNormal += mul(vsIn.normal, (float3x3) jointsTransformMatrix[jointId]) * jointWeight;
			finalTangent += mul(vsIn.tangent, (float3x3) jointsTransformMatrix[jointId]) * jointWeight;
			finalBinormal += mul(vsIn.binormal, (float3x3) jointsTransformMatrix[jointId]) * jointWeight;
		}
	}
	// If mesh deformer isn't used, just take incoming vertex position.
	else {
		finalWorldPosition = float4(vsIn.position, 1.0f);
		finalNormal = vsIn.normal;
		finalTangent = vsIn.tangent;
		finalBinormal = vsIn.binormal;
	}

	//// Apply "Model, View, Projection" transform matrices.
	// Apply "Model" matrix.
	finalWorldPosition = mul(finalWorldPosition, worldMatrix);

	// Apply "View" and "Projection" transform matrices.
	vsOut.position = mul(finalWorldPosition, viewMatrix);
	vsOut.position = mul(vsOut.position, projectionMatrix);

	// Apply shadow map projection & view matrices.
	// These values will be processed for shadowing.
	float4 shadowMapPosition = float4(0,0,0,1);
	for (unsigned int sc = 0; sc < MAX_SHADOW_CASTER_COUNT; sc++) {
		if (shadowMaps[sc].isActive) {
			// Choose which subfrustum to sample from.
			unsigned int sf = 0;
			for (sf = 0; sf < MAX_CSM_SUBFRUSTUM_COUNT; sf++) {
				if (vsOut.position.z <= shadowMaps[sc].subfrustum[sf].activeCameraSubfrustumFarPlaneDistance) {
					vsOut.subFrustumIndices[sc] = sf;
					break;
				}
			}

			// Calculate UV position of current vertex in shadow map.
			for (sf = 0; sf < MAX_CSM_SUBFRUSTUM_COUNT; sf++) {
				shadowMapPosition = mul(finalWorldPosition, shadowMaps[sc].subfrustum[sf].viewMatrix);
				shadowMapPosition = mul(shadowMapPosition, shadowMaps[sc].subfrustum[sf].projectionMatrix);
				vsOut.shadowMapPosition[sc * MAX_CSM_SUBFRUSTUM_COUNT + sf] = shadowMapPosition;
			}
		}
	}

	// Pixel shader needs just world positions without Projection and View.
	vsOut.positionPS = finalWorldPosition.xyz;

	// Rotate the normals with joint and world matrices.
	// When this normal vector passed to pixel shader, it will be interpolated by rasterizer.
	finalNormal = mul(finalNormal, (float3x3) worldMatrix);
	finalTangent = mul(finalTangent, (float3x3) worldMatrix);
	finalBinormal = mul(finalBinormal, (float3x3) worldMatrix);
	vsOut.normal = finalNormal;
	vsOut.tangent = finalTangent;
	vsOut.binormal = finalBinormal;

	vsOut.texture_UV = vsIn.texture_UV;
	vsOut.eyePosition = cameraPosition;

	return vsOut;
}