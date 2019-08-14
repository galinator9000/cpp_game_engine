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

	vso.pos = mul(
		float4(pos.x, pos.y, pos.z, 1.0f),
		worldMatrix
	);

	return vso;
}