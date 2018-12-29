#include "benchmark/benchmark.h"
#include <vector>

const unsigned int Capacity = 1024;

static void BM_VectorCreation(benchmark::State &state)
{
	for (auto _ : state)
	{
		std::vector<unsigned int> vector(state.range(0));
		benchmark::DoNotOptimize(vector);
	}
}
BENCHMARK(BM_VectorCreation)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_VectorCopy(benchmark::State &state)
{
	std::vector<unsigned int> initVector(state.range(0));
	for (unsigned int i = 0; i < state.range(0); i++)
		initVector.push_back(i);
	std::vector<unsigned int> vector(state.range(0));

	for (auto _ : state)
	{
		vector = initVector;
		benchmark::DoNotOptimize(vector);

		state.PauseTiming();
		vector.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_VectorCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_VectorPushBack(benchmark::State &state)
{
	std::vector<unsigned int> vector(state.range(0));

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			vector.push_back(i);
			benchmark::DoNotOptimize(vector);
		}

		state.PauseTiming();
		vector.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_VectorPushBack)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_VectorIterate(benchmark::State &state)
{
	std::vector<unsigned int> vector(state.range(0));
	for (unsigned int i = 0; i < state.range(0); i++)
		vector.push_back(i);

	for (auto _ : state)
	{
		for (unsigned int i : vector)
		{
			unsigned int value = i;
			benchmark::DoNotOptimize(value);
		}
	}
}
BENCHMARK(BM_VectorIterate)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_VectorClear(benchmark::State &state)
{
	std::vector<unsigned int> initVector(state.range(0));
	for (unsigned int i = 0; i < state.range(0); i++)
		initVector.push_back(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		std::vector<unsigned int> vector(initVector);
		state.ResumeTiming();

		vector.clear();
	}
}
BENCHMARK(BM_VectorClear)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_VectorErase(benchmark::State &state)
{
	std::vector<unsigned int> initVector(state.range(0));
	for (unsigned int i = 0; i < state.range(0); i++)
		initVector.push_back(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		std::vector<unsigned int> vector(initVector);
		state.ResumeTiming();

		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(vector.erase(vector.end() - 1));
	}
}
BENCHMARK(BM_VectorErase)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_VectorReverseErase(benchmark::State &state)
{
	std::vector<unsigned int> initVector(state.range(0));
	for (unsigned int i = 0; i < state.range(0); i++)
		initVector.push_back(i);

	for (auto _ : state)
	{
		state.PauseTiming();
		std::vector<unsigned int> vector(initVector);
		state.ResumeTiming();

		for (int i = state.range(0) - 1; i >= 0; i--)
			benchmark::DoNotOptimize(vector.erase(vector.begin()));
	}
}
BENCHMARK(BM_VectorReverseErase)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

BENCHMARK_MAIN();
