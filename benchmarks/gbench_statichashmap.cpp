#include "benchmark/benchmark.h"
#include <nctl/StaticHashMap.h>

const unsigned int Capacity = 1024;
const int KeyValueDifference = 10;

using SaxHashMap = nctl::StaticHashMap<unsigned int, unsigned int, Capacity, nctl::SaxHashFunc<unsigned int> >;
using JenkinsHashMap = nctl::StaticHashMap<unsigned int, unsigned int, Capacity, nctl::JenkinsHashFunc<unsigned int> >;
using FNV1aHashMap = nctl::StaticHashMap<unsigned int, unsigned int, Capacity, nctl::FNV1aHashFunc<unsigned int> >;
using HashMapTestType = FNV1aHashMap;

static void BM_StaticHashMapCreation(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	for (auto _ : state)
	{
		HashMapTestType map;
		benchmark::DoNotOptimize(map);
	}
}
BENCHMARK(BM_StaticHashMapCreation);

static void BM_StaticHashMapCopy(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType initMap;
	for (unsigned int i = 0; i < state.range(0); i++)
		initMap[i] = i * 2;
	HashMapTestType map;

	for (auto _ : state)
	{
		map = initMap;
		benchmark::DoNotOptimize(map);

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_StaticHashMapCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StaticHashMapInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType map;

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(map[i] = i + KeyValueDifference);

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_StaticHashMapInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StaticHashMapRetrieve(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType map;
	for (unsigned int i = 0; i < state.range(0); i++)
		map[i] = i * 2;

	unsigned int key = 0;
	for (auto _ : state)
	{
		key = (key + 19) % state.range(0);
		benchmark::DoNotOptimize(map[key]);
	}
}
BENCHMARK(BM_StaticHashMapRetrieve)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StaticHashMapClear(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType initMap;
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
BENCHMARK(BM_StaticHashMapClear)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StaticHashMapRemove(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType initMap;
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
BENCHMARK(BM_StaticHashMapRemove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StaticHashMapReverseRemove(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType initMap;
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
BENCHMARK(BM_StaticHashMapReverseRemove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

BENCHMARK_MAIN();
