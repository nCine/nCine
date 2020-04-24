#include "benchmark/benchmark.h"
#include <nctl/List.h>
#define TEST_WITH_NCTL
#include "test_movable.h"

const unsigned int Length = 256;

static void BM_BigListCreation(benchmark::State &state)
{
	for (auto _ : state)
	{
		nctl::List<Movable> list;
		benchmark::DoNotOptimize(list);
	}
}
BENCHMARK(BM_BigListCreation);

static void BM_BigListCopy(benchmark::State &state)
{
	nctl::List<Movable> initList;
	for (unsigned int i = 0; i < state.range(0); i++)
		initList.pushBack(Movable(Movable::Construction::INITIALIZED));
	nctl::List<Movable> list;

	for (auto _ : state)
	{
		list = initList;
		benchmark::DoNotOptimize(list);

		state.PauseTiming();
		list.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigListCopy)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

static void BM_BigListMove(benchmark::State &state)
{
	nctl::List<Movable> initList;
	for (unsigned int i = 0; i < state.range(0); i++)
		initList.pushBack(Movable(Movable::Construction::INITIALIZED));
	nctl::List<Movable> list;

	for (auto _ : state)
	{
		list = nctl::move(initList);
		benchmark::DoNotOptimize(list);

		state.PauseTiming();
		list.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigListMove)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

static void BM_BigListPushBack(benchmark::State &state)
{
	nctl::List<Movable> list;

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			Movable movable(Movable::Construction::INITIALIZED);
			list.pushBack(movable);
			benchmark::DoNotOptimize(list);
		}

		state.PauseTiming();
		list.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigListPushBack)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

static void BM_BigListMovePushBack(benchmark::State &state)
{
	nctl::List<Movable> list;

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			Movable movable(Movable::Construction::INITIALIZED);
			list.pushBack(nctl::move(movable));
			benchmark::DoNotOptimize(list);
		}

		state.PauseTiming();
		list.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigListMovePushBack)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

static void BM_BigListEmplaceBack(benchmark::State &state)
{
	nctl::List<Movable> list;

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			list.emplaceBack(Movable::Construction::INITIALIZED);
			benchmark::DoNotOptimize(list);
		}

		state.PauseTiming();
		list.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigListEmplaceBack)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

BENCHMARK_MAIN();
