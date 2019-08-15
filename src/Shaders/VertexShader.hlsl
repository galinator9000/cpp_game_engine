struct VSOut {
	float4 pos : SV_Position;
};

cbuffer ConstantBuffer : register(b0) {
	matrix worldMatrix;
};

cbuffer ViewProjectionMatrices : register(b1) {
	matrix viewMatrix;
	matrix projectionMatrix;
};

VSOut main(float3 pos : Position) {
	VSOut vso;

	float4 finalVector;
	finalVector = mul(
		float4(pos.x, pos.y, pos.z, 1.0f), worldMatrix
	);
	finalVector = mul(finalVector, viewMatrix);
	finalVector = mul(finalVector, projectionMatrix);

	vso.pos = finalVector;
	return vso;
}