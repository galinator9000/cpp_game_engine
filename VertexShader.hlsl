struct VSOut{
	float4 color : Color;
	float4 pos : SV_Position;
};

cbuffer ConstantBuffer{
	matrix transformation;
};

VSOut main(float3 pos : Position, float4 color : Color){
	VSOut vso;
	vso.pos = mul(float4(pos.x, pos.y, pos.z, 1.0f), transformation);
	vso.color = color;
	return vso;
}