float4 main(float3 position : Position) : SV_Position{
	return float4(position, 1.0f);
}