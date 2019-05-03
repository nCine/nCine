#include "benchmark/benchmark.h"
#include <nctl/HashMapList.h>
#define TEST_WITH_NCTL
#include "test_movable.h"

const unsigned int Capacity = 1024;
const int KeyValueDifference = 10;

using SaxHashMap = nctl::HashMapList<unsigned int, Movable, nctl::SaxHashFunc<unsigned int>>;
using JenkinsHashMap = nctl::HashMapList<unsigned int, Movable, nctl::JenkinsHashFunc<unsigned int>>;
using FNV1aHashMap = nctl::HashMapList<unsigned int, Movable, nctl::FNV1aHashFunc<unsigned int>>;
using HashMapTestType = FNV1aHashMap;

static void BM_BigHashMapListCopy(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType initMap(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initMap[i] = nctl::move(Movable(Movable::Construction::INITIALIZED));
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
BENCHMARK(BM_BigHashMapListCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashMapListMove(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType initMap(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initMap[i] = nctl::move(Movable(Movable::Construction::INITIALIZED));
	HashMapTestType map(Capacity);

	for (auto _ : state)
	{
		map = nctl::move(initMap);
		benchmark::DoNotOptimize(map);

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigHashMapListMove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashMapListOperatorInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType map(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			Movable movable(Movable::Construction::INITIALIZED);
			map[i] = movable;
		}

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigHashMapListOperatorInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashMapListOperatorMoveInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType map(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			Movable movable(Movable::Construction::INITIALIZED);
			map[i] = nctl::move(movable);
		}

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigHashMapListOperatorMoveInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashMapListInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType map(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			Movable movable(Movable::Construction::INITIALIZED);
			map.insert(i, movable);
		}

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigHashMapListInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashMapListMoveInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType map(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			Movable movable(Movable::Construction::INITIALIZED);
			map.insert(i, nctl::move(movable));
		}

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigHashMapListMoveInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashMapListEmplace(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	HashMapTestType map(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			map.emplace(i, Movable::Construction::INITIALIZED);

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigHashMapListEmplace)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

BENCHMARK_MAIN();
