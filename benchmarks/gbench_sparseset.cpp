#include "benchmark/benchmark.h"
#include <nctl/SparseSet.h>

const unsigned int Capacity = 1024;
const unsigned int MaxValue = 2048;

using SparseSetTestType = nctl::SparseSet<int>;

static void BM_SparseSetCreation(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	state.counters["MaxValue"] = MaxValue;
	for (auto _ : state)
	{
		SparseSetTestType set(Capacity, MaxValue);
		benchmark::DoNotOptimize(set);
	}
}
BENCHMARK(BM_SparseSetCreation);

static void BM_SparseSetCopy(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	state.counters["MaxValue"] = MaxValue;
	SparseSetTestType initSet(Capacity, MaxValue);
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);
	SparseSetTestType set(Capacity, MaxValue);

	for (auto _ : state)
	{
		set = initSet;
		benchmark::DoNotOptimize(set);

		state.PauseTiming();
		set.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_SparseSetCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_SparseSetInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	state.counters["MaxValue"] = MaxValue;
	SparseSetTestType set(Capacity, MaxValue);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			set.insert(i);

		state.PauseTiming();
		set.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_SparseSetInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_SparseSetRetrieve(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	state.counters["MaxValue"] = MaxValue;
	SparseSetTestType set(Capacity, MaxValue);
	for (unsigned int i = 0; i < state.range(0); i++)
		set.insert(i);

	unsigned int key = 0;
	for (auto _ : state)
	{
		key = (key + 19) % state.range(0);
		benchmark::DoNotOptimize(set.find(key));
	}
}
BENCHMARK(BM_SparseSetRetrieve)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_SparseSetClear(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	state.counters["MaxValue"] = MaxValue;
	SparseSetTestType initSet(Capacity, MaxValue);
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		SparseSetTestType set(initSet);
		state.ResumeTiming();

		set.clear();
	}
}
BENCHMARK(BM_SparseSetClear)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_SparseSetRemove(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	state.counters["MaxValue"] = MaxValue;
	SparseSetTestType initSet(Capacity, MaxValue);
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		SparseSetTestType set(initSet);
		state.ResumeTiming();

		for (unsigned int i = 0; i < state.range(0); i++)
			set.remove(i);
	}
}
BENCHMARK(BM_SparseSetRemove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_SparseSetReverseRemove(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	state.counters["MaxValue"] = MaxValue;
	SparseSetTestType initSet(Capacity, MaxValue);
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		SparseSetTestType set(initSet);
		state.ResumeTiming();

		for (int i = state.range(0) - 1; i >= 0; i--)
			set.remove(i);
	}
}
BENCHMARK(BM_SparseSetReverseRemove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_SparseSetRehashDoubleCapacity(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	state.counters["MaxValue"] = MaxValue;
	SparseSetTestType initSet(Capacity, MaxValue);
	for (unsigned int i = 0; i < state.range(0); i++)
		initSet.insert(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		SparseSetTestType set(initSet);
		state.ResumeTiming();

		set.rehash(Capacity * 2);
	}
}
BENCHMARK(BM_SparseSetRehashDoubleCapacity)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

BENCHMARK_MAIN();
