#include "benchmark/benchmark.h"
#include <unordered_map>

const unsigned int Capacity = 1024;
const int KeyValueDifference = 10;

using StdUnorderedMap = std::unordered_map<unsigned int, unsigned int>;

static void BM_StdUnorderedMapCreation(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	for (auto _ : state)
	{
		StdUnorderedMap map(Capacity);
		benchmark::DoNotOptimize(map);
	}
}
BENCHMARK(BM_StdUnorderedMapCreation);

static void BM_StdUnorderedMapCopy(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedMap initMap(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initMap[i] = i * 2;
	StdUnorderedMap map(Capacity);

	for (auto _ : state)
	{
		map = initMap;
		benchmark::DoNotOptimize(map);

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_StdUnorderedMapCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StdUnorderedMapInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedMap map(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(map[i] = i + KeyValueDifference);

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_StdUnorderedMapInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StdUnorderedMapRetrieve(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedMap map(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		map[i] = i * 2;

	unsigned int key = 0;
	for (auto _ : state)
	{
		key = (key + 19) % state.range(0);
		benchmark::DoNotOptimize(map[key]);
	}
}
BENCHMARK(BM_StdUnorderedMapRetrieve)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StdUnorderedMapClear(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedMap initMap(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initMap[i] = i * 2;

	for (auto _ : state)
	{
		state.PauseTiming();
		StdUnorderedMap map(initMap);
		state.ResumeTiming();

		map.clear();
	}
}
BENCHMARK(BM_StdUnorderedMapClear)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StdUnorderedMapErase(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedMap initMap(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initMap[i] = i * 2;

	for (auto _ : state)
	{
		state.PauseTiming();
		StdUnorderedMap map(initMap);
		state.ResumeTiming();

		for (unsigned int i = 0; i < state.range(0); i++)
			map.erase(i);
	}
}
BENCHMARK(BM_StdUnorderedMapErase)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_StdUnorderedMapReverseErase(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedMap initMap(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initMap[i] = i * 2;

	for (auto _ : state)
	{
		state.PauseTiming();
		StdUnorderedMap map(initMap);
		state.ResumeTiming();

		for (int i = state.range(0) - 1; i >= 0; i--)
			map.erase(i);
	}
}
BENCHMARK(BM_StdUnorderedMapReverseErase)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

BENCHMARK_MAIN();
