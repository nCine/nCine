#include "benchmark/benchmark.h"
#include <nctl/StaticHashSet.h>

const unsigned int Capacity = 1024;

using SaxHashSet = nctl::StaticHashSet<unsigned int, Capacity, nctl::SaxHashFunc<unsigned int>>;
using JenkinsHashSet = nctl::StaticHashSet<unsigned int, Capacity, nctl::JenkinsHashFunc<unsigned int>>;
using FNV1aHashSet = nctl::StaticHashSet<unsigned int, Capacity, nctl::FNV1aHashFunc<unsigned int>>;
using HashSetTestType = FNV1aHashSet;

static void BM_StaticHashSetCreation(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	for (auto _ : state)
	{
		HashSetTestType set;
		benchmark::DoNotOptimize(set);
	}
}
BENCHMARK(BM_StaticHashSetCreation);

static void BM_StaticHashSetCopy(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType initSet;
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);
	HashSetTestType set;

	for (auto _ : state)
	{
		set = initSet;
		benchmark::DoNotOptimize(set);

		state.PauseTiming();
		set.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_StaticHashSetCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StaticHashSetInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType set;

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			set.insert(i);

		state.PauseTiming();
		set.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_StaticHashSetInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StaticHashSetRetrieve(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType set;
	for (unsigned int i = 0; i < state.range(0); i++)
		set.insert(i);

	unsigned int key = 0;
	for (auto _ : state)
	{
		key = (key + 19) % state.range(0);
		benchmark::DoNotOptimize(set.find(key));
	}
}
BENCHMARK(BM_StaticHashSetRetrieve)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StaticHashSetClear(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType initSet;
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		HashSetTestType set(initSet);
		state.ResumeTiming();

		set.clear();
	}
}
BENCHMARK(BM_StaticHashSetClear)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StaticHashSetRemove(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType initSet;
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		HashSetTestType set(initSet);
		state.ResumeTiming();

		for (unsigned int i = 0; i < state.range(0); i++)
			set.remove(i);
	}
}
BENCHMARK(BM_StaticHashSetRemove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StaticHashSetReverseRemove(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType initSet;
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		HashSetTestType set(initSet);
		state.ResumeTiming();

		for (int i = state.range(0) - 1; i >= 0; i--)
			set.remove(i);
	}
}
BENCHMARK(BM_StaticHashSetReverseRemove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

BENCHMARK_MAIN();
