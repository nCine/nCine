struct VSInput
{
	float2 position : TEXCOORD0;
	float4 color    : TEXCOORD1;
};

struct VSOutput
{
	float3 fragColor : TEXCOORD0;
	float  pointSize : SV_PointSize;
	float4 position  : SV_Position;
};

[shader("vertex")]
VSOutput main(VSInput input)
{
	VSOutput o;

	o.position  = float4(input.position.xy, 1.0, 1.0);
	o.fragColor = input.color.rgb;
	o.pointSize = 14.0;

	return o;
}
