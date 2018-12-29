#include "benchmark/benchmark.h"
#include "nctl/StaticArray.h"

const unsigned int Capacity = 1024;

static void BM_StaticArrayCreation(benchmark::State &state)
{
	for (auto _ : state)
	{
		nctl::StaticArray<unsigned int, Capacity> array;
		benchmark::DoNotOptimize(array);
	}
}
BENCHMARK(BM_StaticArrayCreation);

static void BM_StaticArrayCopy(benchmark::State &state)
{
	nctl::StaticArray<unsigned int, Capacity> initArray;
	for (unsigned int i = 0; i < Capacity; i++)
		initArray[i] = i;
	nctl::StaticArray<unsigned int, Capacity> array;

	for (auto _ : state)
	{
		array = initArray;
		benchmark::DoNotOptimize(array);
	}
}
BENCHMARK(BM_StaticArrayCopy);

static void BM_StaticArraySubscriptOperator(benchmark::State &state)
{
	nctl::StaticArray<unsigned int, Capacity> array;

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < Capacity; i++)
		{
			array[i] = i;
			benchmark::DoNotOptimize(array);
		}
	}
}
BENCHMARK(BM_StaticArraySubscriptOperator);

static void BM_StaticArrayIterate(benchmark::State &state)
{
	nctl::StaticArray<unsigned int, Capacity> array;
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
BENCHMARK(BM_StaticArrayIterate);

BENCHMARK_MAIN();
