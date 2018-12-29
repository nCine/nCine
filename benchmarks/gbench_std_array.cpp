#include "benchmark/benchmark.h"
#include <array>

const unsigned int Capacity = 1024;

static void BM_ArrayCreation(benchmark::State &state)
{
	for (auto _ : state)
	{
		std::array<unsigned int, Capacity> array;
		benchmark::DoNotOptimize(array);
	}
}
BENCHMARK(BM_ArrayCreation);

static void BM_ArrayCopy(benchmark::State &state)
{
	std::array<unsigned int, Capacity> initArray;
	for (unsigned int i = 0; i < Capacity; i++)
		initArray[i] = i;
	std::array<unsigned int, Capacity> array;

	for (auto _ : state)
	{
		array = initArray;
		benchmark::DoNotOptimize(array);
	}
}
BENCHMARK(BM_ArrayCopy);

static void BM_ArraySubscriptOperator(benchmark::State &state)
{
	std::array<unsigned int, Capacity> array;

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < Capacity; i++)
		{
			array[i] = i;
			benchmark::DoNotOptimize(array);
		}
	}
}
BENCHMARK(BM_ArraySubscriptOperator);

static void BM_ArrayIterate(benchmark::State &state)
{
	std::array<unsigned int, Capacity> array;
	for (unsigned int i = 0; i < Capacity; i++)
		array[i] = i;

	for (auto _ : state)
	{
		for (unsigned int i : array)
		{
			unsigned int value = i;
			benchmark::DoNotOptimize(value);
		}
	}
}
BENCHMARK(BM_ArrayIterate);

BENCHMARK_MAIN();
