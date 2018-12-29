#include "benchmark/benchmark.h"
#include "nctl/HashMap.h"

const unsigned int Capacity = 1024;
const int KeyValueDifference = 10;

using SaxHashMap = nctl::HashMap<unsigned int, unsigned int, nctl::SaxHashFunc<unsigned int> >;
using JenkinsHashMap = nctl::HashMap<unsigned int, unsigned int, nctl::JenkinsHashFunc<unsigned int> >;
using FNV1aHashMap = nctl::HashMap<unsigned int, unsigned int, nctl::FNV1aHashFunc<unsigned int> >;
using HashMapTestType = FNV1aHashMap;

static void BM_HashMapCreation(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	for (auto _ : state)
	{
		HashMapTestType map(Capacity);
		benchmark::DoNotOptimize(map);
	}
}
BENCHMARK(BM_HashMapCreation);

static void BM_HashMapCopy(benchmark::State &state)
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
BENCHMARK(BM_HashMapCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashMapInsert(benchmark::State &state)
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
BENCHMARK(BM_HashMapInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashMapRetrieve(benchmark::State &state)
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
BENCHMARK(BM_HashMapRetrieve)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashMapClear(benchmark::State &state)
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
BENCHMARK(BM_HashMapClear)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashMapRemove(benchmark::State &state)
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
BENCHMARK(BM_HashMapRemove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_HashMapReverseRemove(benchmark::State &state)
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
BENCHMARK(BM_HashMapReverseRemove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

BENCHMARK_MAIN();
