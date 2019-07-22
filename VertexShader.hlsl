struct VSOut{
	float4 color : Color;
	float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float4 color : Color){
	VSOut vso;
	vso.pos = float4(pos.x, pos.y, pos.z, 1.0f);
	vso.color = color;
	return vso;
}