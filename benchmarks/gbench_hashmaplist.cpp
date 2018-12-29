#include "benchmark/benchmark.h"
#include "nctl/HashMapList.h"

const unsigned int Capacity = 1024;
const int KeyValueDifference = 10;

using SaxHashMap = nctl::HashMapList<unsigned int, unsigned int, nctl::SaxHashFunc<unsigned int> >;
using JenkinsHashMap = nctl::HashMapList<unsigned int, unsigned int, nctl::JenkinsHashFunc<unsigned int> >;
using FNV1aHashMap = nctl::HashMapList<unsigned int, unsigned int, nctl::FNV1aHashFunc<unsigned int> >;
using HashMapTestType = FNV1aHashMap;

static void BM_HashMapListCreation(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	for (auto _ : state)
	{
		HashMapTestType map(Capacity);
		benchmark::DoNotOptimize(map);
	}
}
BENCHMARK(BM_HashMapListCreation);

static void BM_HashMapListCopy(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType initMap(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initMap[i] = i * 2;
	HashMapTestType map(Capacity);

	for (auto _ : state)
	{
		map = initMap;
		benchmark::DoNotOptimize(map);

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_HashMapListCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashMapListInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType map(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(map[i] = i + KeyValueDifference);

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_HashMapListInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashMapListRetrieve(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType map(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		map[i] = i * 2;

	unsigned int key = 0;
	for (auto _ : state)
	{
		key = (key + 19) % state.range(0);
		benchmark::DoNotOptimize(map[key]);
	}
}
BENCHMARK(BM_HashMapListRetrieve)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashMapListClear(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType initMap(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initMap[i] = i * 2;

	for (auto _ : state)
	{
		state.PauseTiming();
		HashMapTestType map(initMap);
		state.ResumeTiming();

		map.clear();
	}
}
BENCHMARK(BM_HashMapListClear)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashMapListRemove(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType initMap(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initMap[i] = i * 2;

	for (auto _ : state)
	{
		state.PauseTiming();
		HashMapTestType map(initMap);
		state.ResumeTiming();

		for (unsigned int i = 0; i < state.range(0); i++)
			map.remove(i);
	}
}
BENCHMARK(BM_HashMapListRemove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashMapListReverseRemove(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType initMap(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initMap[i] = i * 2;

	for (auto _ : state)
	{
		state.PauseTiming();
		HashMapTestType map(initMap);
		state.ResumeTiming();

		for (int i = state.range(0) - 1; i >= 0; i--)
			map.remove(i);
	}
}
BENCHMARK(BM_HashMapListReverseRemove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

BENCHMARK_MAIN();
