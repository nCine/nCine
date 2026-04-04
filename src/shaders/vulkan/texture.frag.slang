[[vk::binding(1, 0)]]
Sampler2D texture : register(s1);

struct PSInput
{
	[[vk::location(0)]]
	float3 fragColor;

	[[vk::location(1)]]
	float2 fragTexCoord;
};

[shader("fragment")]
float4 main(PSInput input) : SV_Target0
{
	return texture.Sample(input.fragTexCoord) * float4(input.fragColor, 1.0);
}
