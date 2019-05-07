#include "benchmark/benchmark.h"
#include <nctl/HashSetList.h>

const unsigned int Capacity = 1024;

using SaxHashSet = nctl::HashSetList<unsigned int, nctl::SaxHashFunc<unsigned int>>;
using JenkinsHashSet = nctl::HashSetList<unsigned int, nctl::JenkinsHashFunc<unsigned int>>;
using FNV1aHashSet = nctl::HashSetList<unsigned int, nctl::FNV1aHashFunc<unsigned int>>;
using HashSetTestType = FNV1aHashSet;

static void BM_HashSetListCreation(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	for (auto _ : state)
	{
		HashSetTestType set(Capacity);
		benchmark::DoNotOptimize(set);
	}
}
BENCHMARK(BM_HashSetListCreation);

static void BM_HashSetListCopy(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType initSet(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);
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
BENCHMARK(BM_HashSetListCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashSetListInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType set(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			set.insert(i);

		state.PauseTiming();
		set.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_HashSetListInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashSetListRetrieve(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType set(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		set.insert(i);

	unsigned int key = 0;
	for (auto _ : state)
	{
		key = (key + 19) % state.range(0);
		benchmark::DoNotOptimize(set.find(key));
	}
}
BENCHMARK(BM_HashSetListRetrieve)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashSetListClear(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType initSet(Capacity);
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
BENCHMARK(BM_HashSetListClear)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashSetListRemove(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType initSet(Capacity);
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
BENCHMARK(BM_HashSetListRemove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashSetListReverseRemove(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType initSet(Capacity);
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
BENCHMARK(BM_HashSetListReverseRemove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashSetListRehashDoubleCapacity(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashSetTestType initSet(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		HashSetTestType set(initSet);
		state.ResumeTiming();

		set.rehash(Capacity * 2);
	}
}
BENCHMARK(BM_HashSetListRehashDoubleCapacity)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

BENCHMARK_MAIN();
