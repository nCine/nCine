struct Particle
{
	float2 position;
	float2 velocity;
	float4 color;
};

[[vk::binding(0, 0)]]
cbuffer ParameterUBO
{
	float deltaTime;
};

[[vk::binding(1, 0)]]
StructuredBuffer<Particle> particlesIn;

[[vk::binding(2, 0)]]
RWStructuredBuffer<Particle> particlesOut;

[numthreads(256, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint index = dispatchThreadID.x;

	Particle particleIn = particlesIn[index];

	float2 newPos = particleIn.position + particleIn.velocity * deltaTime;
	float2 vel = particleIn.velocity;

	if (newPos.x < -1.0)
	{
		float overshoot = -1.0 - newPos.x;
		newPos.x = -1.0 + overshoot;
		vel.x = -vel.x;
	}
	else if (newPos.x > 1.0)
	{
		float overshoot = newPos.x - 1.0;
		newPos.x = 1.0 - overshoot;
		vel.x = -vel.x;
	}

	if (newPos.y < -1.0)
	{
		float overshoot = -1.0 - newPos.y;
		newPos.y = -1.0 + overshoot;
		vel.y = -vel.y;
	}
	else if (newPos.y > 1.0)
	{
		float overshoot = newPos.y - 1.0;
		newPos.y = 1.0 - overshoot;
		vel.y = -vel.y;
	}

	Particle particleOut;
	particleOut.position = newPos;
	particleOut.velocity = vel;
	particleOut.color = particleIn.color;
	particlesOut[index] = particleOut;
}
