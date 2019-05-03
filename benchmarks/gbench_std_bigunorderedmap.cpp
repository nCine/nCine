#include "benchmark/benchmark.h"
#include <unordered_map>
#include "test_movable.h"

const unsigned int Capacity = 1024;
const int KeyValueDifference = 10;

using StdUnorderedMap = std::unordered_map<unsigned int, Movable>;

static void BM_BigStdUnorderedMapCopy(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedMap initMap(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initMap[i] = std::move(Movable(Movable::Construction::INITIALIZED));
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
BENCHMARK(BM_BigStdUnorderedMapCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigStdUnorderedMapMove(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedMap initMap(Capacity);
	for (unsigned int i = 0; i < state.range(0); i++)
		initMap[i] = std::move(Movable(Movable::Construction::INITIALIZED));
	StdUnorderedMap map(Capacity);

	for (auto _ : state)
	{
		map = std::move(initMap);
		benchmark::DoNotOptimize(map);

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigStdUnorderedMapMove)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigStdUnorderedMapOperatorInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedMap map(Capacity);

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
BENCHMARK(BM_BigStdUnorderedMapOperatorInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigStdUnorderedMapOperatorMoveInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedMap map(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			Movable movable(Movable::Construction::INITIALIZED);
			map[i] = std::move(movable);
		}

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigStdUnorderedMapOperatorMoveInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigStdUnorderedMapInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedMap map(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			Movable movable(Movable::Construction::INITIALIZED);
			map.insert(std::make_pair(i, movable));
		}

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigStdUnorderedMapInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigStdUnorderedMapMoveInsert(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedMap map(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			Movable movable(Movable::Construction::INITIALIZED);
			map.insert(std::make_pair(i, std::move(movable)));
		}

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigStdUnorderedMapMoveInsert)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

static void BM_BigStdUnorderedMapEmplace(benchmark::State &state)
{
	state.counters["Capacity"] = Capacity;
	StdUnorderedMap map(Capacity);

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			map.emplace(i, Movable::Construction::INITIALIZED);

		state.PauseTiming();
		map.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_BigStdUnorderedMapEmplace)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity / 4 * 3);

BENCHMARK_MAIN();
