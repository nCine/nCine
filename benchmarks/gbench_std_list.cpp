#include "benchmark/benchmark.h"
#include <list>

const unsigned int Length = 256;

static void BM_ListCreation(benchmark::State &state)
{
	for (auto _ : state)
	{
		std::list<unsigned int> list;
		benchmark::DoNotOptimize(list);
	}
}
BENCHMARK(BM_ListCreation);

static void BM_ListCopy(benchmark::State &state)
{
	std::list<unsigned int> initList;
	for (unsigned int i = 0; i < state.range(0); i++)
		initList.push_back(i);
	std::list<unsigned int> list;

	for (auto _ : state)
	{
		list = initList;
		benchmark::DoNotOptimize(list);

		state.PauseTiming();
		list.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_ListCopy)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

static void BM_ListPushBack(benchmark::State &state)
{
	std::list<unsigned int> list;

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			list.push_back(i);
			benchmark::DoNotOptimize(list);
		}

		state.PauseTiming();
		list.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_ListPushBack)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

static void BM_ListIterate(benchmark::State &state)
{
	std::list<unsigned int> list;
	for (unsigned int i = 0; i < state.range(0); i++)
		list.push_back(i);

	for (auto _ : state)
	{
		for (unsigned int i : list)
		{
			unsigned int value = i;
			benchmark::DoNotOptimize(value);
		}
	}
}
BENCHMARK(BM_ListIterate)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

static void BM_ListClear(benchmark::State &state)
{
	std::list<unsigned int> initList;
	for (unsigned int i = 0; i < state.range(0); i++)
		initList.push_back(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		std::list<unsigned int> list(initList);
		state.ResumeTiming();

		list.clear();
	}
}
BENCHMARK(BM_ListClear)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

static void BM_ListRemove(benchmark::State &state)
{
	std::list<unsigned int> initList;
	for (unsigned int i = 0; i < state.range(0); i++)
		initList.push_back(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		std::list<unsigned int> list(initList);
		state.ResumeTiming();

		for (unsigned int i = 0; i < state.range(0); i++)
			list.remove(i);
	}
}
BENCHMARK(BM_ListRemove)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

static void BM_ListReverseRemove(benchmark::State &state)
{
	std::list<unsigned int> initList;
	for (unsigned int i = 0; i < state.range(0); i++)
		initList.push_back(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		std::list<unsigned int> list(initList);
		state.ResumeTiming();

		for (int i = state.range(0) - 1; i >= 0; i--)
			list.remove(i);
	}
}
BENCHMARK(BM_ListReverseRemove)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

BENCHMARK_MAIN();
