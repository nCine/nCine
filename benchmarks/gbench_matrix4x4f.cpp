#include "benchmark/benchmark.h"
#include <ncine/Matrix4x4.h>

const unsigned int Repetitions = 12;

const float translationX = 10.0f;
const float translationY = 15.0f;
const float rotationZ = 45.0f;
const float scalingX = 2.0f;
const float scalingY = 1.5f;
const float anchorX = -5.0f;
const float anchorY = 10.0f;

static void BM_TransformNodeFromIdentity(benchmark::State &state)
{
	ncine::Matrix4x4f matrix;

	for (auto _ : state)
	{
		matrix = ncine::Matrix4x4f::Identity;
		matrix *= ncine::Matrix4x4f::translation(translationX, translationY, 0.0f);
		matrix *= ncine::Matrix4x4f::rotationZ(rotationZ);
		matrix *= ncine::Matrix4x4f::scaling(scalingX, scalingY, 1.0f);
		matrix *= ncine::Matrix4x4f::translation(-anchorX, -anchorY, 0.0f);
		benchmark::DoNotOptimize(matrix);
	}
}
BENCHMARK(BM_TransformNodeFromIdentity);

static void BM_TransformNode(benchmark::State &state)
{
	ncine::Matrix4x4f matrix;

	for (auto _ : state)
	{
		matrix = ncine::Matrix4x4f::translation(translationX, translationY, 0.0f);
		matrix *= ncine::Matrix4x4f::rotationZ(rotationZ);
		matrix *= ncine::Matrix4x4f::scaling(scalingX, scalingY, 1.0f);
		matrix *= ncine::Matrix4x4f::translation(-anchorX, -anchorY, 0.0f);
		benchmark::DoNotOptimize(matrix);
	}
}
BENCHMARK(BM_TransformNode);

static void BM_TransformNodeInPlace(benchmark::State &state)
{
	ncine::Matrix4x4f matrix;

	for (auto _ : state)
	{
		matrix = ncine::Matrix4x4f::translation(translationX, translationY, 0.0f);
		matrix.rotateZ(rotationZ);
		matrix.scale(scalingX, scalingY, 1.0f);
		matrix.translate(-anchorX, -anchorY, 0.0f);
		benchmark::DoNotOptimize(matrix);
	}
}
BENCHMARK(BM_TransformNodeInPlace);

static void BM_ManyTransformationsFromIdentity(benchmark::State &state)
{
	ncine::Matrix4x4f matrix;

	for (auto _ : state)
	{
		matrix = ncine::Matrix4x4f::Identity;

		for (unsigned int i = 0; i < state.range(0); i++)
		{
			matrix *= ncine::Matrix4x4f::translation(translationX, translationY, 0.0f);
			matrix *= ncine::Matrix4x4f::rotationX(-rotationZ);
			matrix *= ncine::Matrix4x4f::rotationY(2.0f * rotationZ);
			matrix *= ncine::Matrix4x4f::rotationZ(rotationZ);
			matrix *= ncine::Matrix4x4f::scaling(scalingX, scalingY, 1.0f);
			matrix *= ncine::Matrix4x4f::translation(-anchorX, -anchorY, 0.0f);
			matrix *= ncine::Matrix4x4f::scaling(-scalingY, scalingX, 1.0f);
			matrix *= ncine::Matrix4x4f::translation(translationY * 2.0f, 0.5f * translationX, 0.0f);
		}
		benchmark::DoNotOptimize(matrix);
	}
}
BENCHMARK(BM_ManyTransformationsFromIdentity)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

static void BM_ManyTransformations(benchmark::State &state)
{
	ncine::Matrix4x4f matrix;

	for (auto _ : state)
	{
		matrix = ncine::Matrix4x4f::translation(translationX, translationY, 0.0f);

		for (unsigned int i = 0; i < state.range(0); i++)
		{
			matrix *= ncine::Matrix4x4f::rotationX(-rotationZ);
			matrix *= ncine::Matrix4x4f::rotationY(2.0f * rotationZ);
			matrix *= ncine::Matrix4x4f::rotationZ(rotationZ);
			matrix *= ncine::Matrix4x4f::scaling(scalingX, scalingY, 1.0f);
			matrix *= ncine::Matrix4x4f::translation(-anchorX, -anchorY, 0.0f);
			matrix *= ncine::Matrix4x4f::scaling(-scalingY, scalingX, 1.0f);
			matrix *= ncine::Matrix4x4f::translation(translationY * 2.0f, 0.5f * translationX, 0.0f);
		}
		benchmark::DoNotOptimize(matrix);
	}
}
BENCHMARK(BM_ManyTransformations)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

static void BM_ManyTransformationsInPlace(benchmark::State &state)
{
	ncine::Matrix4x4f matrix;

	for (auto _ : state)
	{
		matrix = ncine::Matrix4x4f::translation(translationX, translationY, 0.0f);

		for (unsigned int i = 0; i < state.range(0); i++)
		{
			matrix.rotateX(-rotationZ);
			matrix.rotateY(2.0f * rotationZ);
			matrix.rotateZ(rotationZ);
			matrix.scale(scalingX, scalingY, 1.0f);
			matrix.translate(-anchorX, -anchorY, 0.0f);
			matrix.scale(-scalingY, scalingX, 1.0f);
			matrix.translate(translationY * 2.0f, 0.5f * translationX, 0.0f);
		}

		benchmark::DoNotOptimize(matrix);
	}
}
BENCHMARK(BM_ManyTransformationsInPlace)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

BENCHMARK_MAIN();
