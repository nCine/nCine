#include "benchmark/benchmark.h"
#include <nctl/HashMap.h>
#define TEST_WITH_NCTL
#include "test_movable.h"

const unsigned int Capacity = 1024;
const int KeyValueDifference = 10;

using SaxHashMap = nctl::HashMap<unsigned int, Movable, nctl::SaxHashFunc<unsigned int>>;
using JenkinsHashMap = nctl::HashMap<unsigned int, Movable, nctl::JenkinsHashFunc<unsigned int>>;
using FNV1aHashMap = nctl::HashMap<unsigned int, Movable, nctl::FNV1aHashFunc<unsigned int>>;
using HashMapTestType = FNV1aHashMap;

static void BM_BigHashMapCopy(benchmark::State &state)
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
BENCHMARK(BM_BigHashMapCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashMapMove(benchmark::State &state)
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
BENCHMARK(BM_BigHashMapMove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashMapOperatorInsert(benchmark::State &state)
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
BENCHMARK(BM_BigHashMapOperatorInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashMapOperatorMoveInsert(benchmark::State &state)
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
BENCHMARK(BM_BigHashMapOperatorMoveInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashMapInsert(benchmark::State &state)
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
BENCHMARK(BM_BigHashMapInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashMapMoveInsert(benchmark::State &state)
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
BENCHMARK(BM_BigHashMapMoveInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigHashMapEmplace(benchmark::State &state)
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
BENCHMARK(BM_BigHashMapEmplace)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

BENCHMARK_MAIN();
