struct Uniforms
{
	float4x4 model;
	float4x4 view;
	float4x4 projection;
};

[[vk::binding(0, 0)]]
ConstantBuffer<Uniforms> ubo : register(b0);

struct VSInput
{
	[[vk::location(0)]]
	float2 position;

	[[vk::location(1)]]
	float3 color;

	[[vk::location(2)]]
	float2 texCoord;
}

struct VSOutput
{
	[[vk::location(0)]]
	float3 fragColor;

	[[vk::location(1)]]
	float2 fragTexCoord;

	float4 position : SV_Position;
};

[shader("vertex")]
VSOutput main(VSInput input)
{
	VSOutput output;
	float4 pos = float4(input.position, 0.0, 1.0);
	output.position =  mul(ubo.projection, mul(ubo.view, mul(ubo.model, pos)));
	output.fragColor = input.color;
	output.fragTexCoord = input.texCoord;
	return output;
}
