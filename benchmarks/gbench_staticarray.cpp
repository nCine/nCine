#include "benchmark/benchmark.h"
#include <nctl/StaticArray.h>

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

static void BM_StaticArrayPushBack(benchmark::State &state)
{
	nctl::StaticArray<unsigned int, Capacity> array;

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < Capacity; i++)
		{
			array.pushBack(i);
			benchmark::DoNotOptimize(array);
		}

		state.PauseTiming();
		array.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_StaticArrayPushBack);

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

static void BM_StaticArrayClear(benchmark::State &state)
{
	nctl::StaticArray<unsigned int, Capacity> initArray;
	for (unsigned int i = 0; i < Capacity; i++)
		initArray.pushBack(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		nctl::StaticArray<unsigned int, Capacity> array(initArray);
		state.ResumeTiming();

		array.clear();
	}
}
BENCHMARK(BM_StaticArrayClear);

static void BM_StaticArrayErase(benchmark::State &state)
{
	nctl::StaticArray<unsigned int, Capacity> initArray;
	for (unsigned int i = 0; i < state.range(0); i++)
		initArray.pushBack(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		nctl::StaticArray<unsigned int, Capacity> array(initArray);
		state.ResumeTiming();

		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(array.erase(array.end() - 1));
	}
}
BENCHMARK(BM_StaticArrayErase)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_StaticArrayReverseErase(benchmark::State &state)
{
	nctl::StaticArray<unsigned int, Capacity> initArray;
	for (unsigned int i = 0; i < state.range(0); i++)
		initArray.pushBack(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		nctl::StaticArray<unsigned int, Capacity> array(initArray);
		state.ResumeTiming();

		for (int i = state.range(0) - 1; i >= 0; i--)
			benchmark::DoNotOptimize(array.erase(array.begin()));
	}
}
BENCHMARK(BM_StaticArrayReverseErase)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

BENCHMARK_MAIN();
