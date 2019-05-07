#include "benchmark/benchmark.h"
#include <nctl/HashSet.h>

const unsigned int Capacity = 1024;

using SaxHashSet = nctl::HashSet<unsigned int, nctl::SaxHashFunc<unsigned int>>;
using JenkinsHashSet = nctl::HashSet<unsigned int, nctl::JenkinsHashFunc<unsigned int>>;
using FNV1aHashSet = nctl::HashSet<unsigned int, nctl::FNV1aHashFunc<unsigned int>>;
using HashSetTestType = FNV1aHashSet;

static void BM_HashSetCreation(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	for (auto _ : state)
	{
		HashSetTestType set(Capacity);
		benchmark::DoNotOptimize(set);
	}
}
BENCHMARK(BM_HashSetCreation);

static void BM_HashSetCopy(benchmark::State &state)
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
BENCHMARK(BM_HashSetCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashSetInsert(benchmark::State &state)
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
BENCHMARK(BM_HashSetInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashSetRetrieve(benchmark::State &state)
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
BENCHMARK(BM_HashSetRetrieve)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashSetClear(benchmark::State &state)
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
BENCHMARK(BM_HashSetClear)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashSetRemove(benchmark::State &state)
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
BENCHMARK(BM_HashSetRemove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashSetReverseRemove(benchmark::State &state)
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
BENCHMARK(BM_HashSetReverseRemove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashSetRehashDoubleCapacity(benchmark::State &state)
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
BENCHMARK(BM_HashSetRehashDoubleCapacity)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

BENCHMARK_MAIN();
