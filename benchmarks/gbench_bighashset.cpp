#include "benchmark/benchmark.h"
#include <nctl/HashSet.h>
#define TEST_WITH_NCTL
#include "test_movable.h"

const unsigned int Capacity = 1024;

using SaxHashSet = nctl::HashSet<Movable, nctl::SaxHashFunc<Movable>>;
using JenkinsHashSet = nctl::HashSet<Movable, nctl::JenkinsHashFunc<Movable>>;
using FNV1aHashSet = nctl::HashSet<Movable, nctl::FNV1aHashFunc<Movable>>;
using HashSetTestType = FNV1aHashSet;

static void BM_BigHashSetCopy(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType initSet(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(nctl::move(Movable(Movable::Construction::INITIALIZED)));
	HashSetTestType set(Capacity);

	for (auto _ : state)
	{
		set = initSet;
		benchmark::DoNotOptimize(set);

		state.PauseTiming();
		set.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigHashSetCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashSetMove(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType initSet(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(nctl::move(Movable(Movable::Construction::INITIALIZED)));
	HashSetTestType set(Capacity);

	for (auto _ : state)
	{
		set = nctl::move(initSet);
		benchmark::DoNotOptimize(set);

		state.PauseTiming();
		set.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigHashSetMove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashSetOperatorInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType set(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			Movable movable(Movable::Construction::INITIALIZED);
			set.insert(movable);
		}

		state.PauseTiming();
		set.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigHashSetOperatorInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashSetOperatorMoveInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType set(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			Movable movable(Movable::Construction::INITIALIZED);
			set.insert(nctl::move(movable));
		}

		state.PauseTiming();
		set.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigHashSetOperatorMoveInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashSetInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType set(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			Movable movable(Movable::Construction::INITIALIZED);
			set.insert(movable);
		}

		state.PauseTiming();
		set.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigHashSetInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashSetMoveInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType set(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			Movable movable(Movable::Construction::INITIALIZED);
			set.insert(nctl::move(movable));
		}

		state.PauseTiming();
		set.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigHashSetMoveInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

BENCHMARK_MAIN();
