struct PSInput
{
	float3 fragColor : TEXCOORD0;
	float2 pointCoord : SV_PointCoord;
};

[shader("fragment")]
float4 main(PSInput input) : SV_Target0
{
	float2 coord = input.pointCoord - float2(0.5, 0.5);
	float d = length(coord);
	float alpha = smoothstep(0.5, 0.0, d);

	return float4(input.fragColor, alpha);
}
