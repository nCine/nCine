#include "benchmark/benchmark.h"
#include <unordered_set>

const unsigned int Capacity = 1024;

using StdUnorderedSet = std::unordered_set<unsigned int>;

static void BM_StdUnorderedSetCreation(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	for (auto _ : state)
	{
		StdUnorderedSet set(Capacity);
		benchmark::DoNotOptimize(set);
	}
}
BENCHMARK(BM_StdUnorderedSetCreation);

static void BM_StdUnorderedSetCopy(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedSet initSet(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);
	StdUnorderedSet set(Capacity);

	for (auto _ : state)
	{
		set = initSet;
		benchmark::DoNotOptimize(set);

		state.PauseTiming();
		set.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_StdUnorderedSetCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StdUnorderedSetInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedSet set(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			set.insert(i);

		state.PauseTiming();
		set.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_StdUnorderedSetInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StdUnorderedSetRetrieve(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedSet set(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		set.insert(i);

	unsigned int key = 0;
	for (auto _ : state)
	{
		key = (key + 19) % state.range(0);
		benchmark::DoNotOptimize(set.find(key));
	}
}
BENCHMARK(BM_StdUnorderedSetRetrieve)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StdUnorderedSetClear(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedSet initSet(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		StdUnorderedSet set(initSet);
		state.ResumeTiming();

		set.clear();
	}
}
BENCHMARK(BM_StdUnorderedSetClear)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StdUnorderedSetErase(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedSet initSet(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		StdUnorderedSet set(initSet);
		state.ResumeTiming();

		for (unsigned int i = 0; i < state.range(0); i++)
			set.erase(i);
	}
}
BENCHMARK(BM_StdUnorderedSetErase)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StdUnorderedSetReverseErase(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedSet initSet(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		StdUnorderedSet set(initSet);
		state.ResumeTiming();

		for (int i = state.range(0) - 1; i >= 0; i--)
			set.erase(i);
	}
}
BENCHMARK(BM_StdUnorderedSetReverseErase)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StdUnorderedSetRehashDoubleCapacity(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedSet initSet(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		StdUnorderedSet set(initSet);
		state.ResumeTiming();

		set.rehash(Capacity * 2);
	}
}
BENCHMARK(BM_StdUnorderedSetRehashDoubleCapacity)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

BENCHMARK_MAIN();
