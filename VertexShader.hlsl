float4 main(float3 pos : Position) : SV_Position
{
	return float4(pos.x, pos.y, pos.z, 1.0f);
}