// Input structure of the Pixel shader.
struct PSIn {
	float4 color : Color;
};

// Output structure of the Pixel shader.
struct PSOut {
	float4 color : SV_Target;
};

PSOut main(PSIn psin){
	PSOut pso;

	pso.color = psin.color;

	return pso;
}