#include "benchmark/benchmark.h"
#include <nctl/Array.h>

const unsigned int Capacity = 1024;

static void BM_ArrayCreation(benchmark::State &state)
{
	for (auto _ : state)
	{
		nctl::Array<unsigned int> array(state.range(0));
		benchmark::DoNotOptimize(array);
	}
}
BENCHMARK(BM_ArrayCreation)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_ArrayCopy(benchmark::State &state)
{
	nctl::Array<unsigned int> initArray(state.range(0));
	for (unsigned int i = 0; i < state.range(0); i++)
		initArray.pushBack(i);
	nctl::Array<unsigned int> array(state.range(0));

	for (auto _ : state)
	{
		array = initArray;
		benchmark::DoNotOptimize(array);

		state.PauseTiming();
		array.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_ArrayCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_ArrayPushBack(benchmark::State &state)
{
	nctl::Array<unsigned int> array(state.range(0));

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			array.pushBack(i);
			benchmark::DoNotOptimize(array);
		}

		state.PauseTiming();
		array.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_ArrayPushBack)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_ArrayIterate(benchmark::State &state)
{
	nctl::Array<unsigned int> array(state.range(0));
	for (unsigned int i = 0; i < state.range(0); i++)
		array.pushBack(i);

	for (auto _ : state)
	{
		for (unsigned int i : array)
		{
			unsigned int value = i;
			benchmark::DoNotOptimize(value);
		}
	}
}
BENCHMARK(BM_ArrayIterate)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_ArrayClear(benchmark::State &state)
{
	nctl::Array<unsigned int> initArray(state.range(0));
	for (unsigned int i = 0; i < state.range(0); i++)
		initArray.pushBack(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		nctl::Array<unsigned int> array(initArray);
		state.ResumeTiming();

		array.clear();
	}
}
BENCHMARK(BM_ArrayClear)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_ArrayErase(benchmark::State &state)
{
	nctl::Array<unsigned int> initArray(state.range(0));
	for (unsigned int i = 0; i < state.range(0); i++)
		initArray.pushBack(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		nctl::Array<unsigned int> array(initArray);
		state.ResumeTiming();

		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(array.erase(array.end() - 1));
	}
}
BENCHMARK(BM_ArrayErase)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_ArrayReverseErase(benchmark::State &state)
{
	nctl::Array<unsigned int> initArray(state.range(0));
	for (unsigned int i = 0; i < state.range(0); i++)
		initArray.pushBack(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		nctl::Array<unsigned int> array(initArray);
		state.ResumeTiming();

		for (int i = state.range(0) - 1; i >= 0; i--)
			benchmark::DoNotOptimize(array.erase(array.begin()));
	}
}
BENCHMARK(BM_ArrayReverseErase)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

BENCHMARK_MAIN();
