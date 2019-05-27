#include "benchmark/benchmark.h"
#include <ctime>
#include <cstdlib>

const unsigned int Repetitions = 1024;

static void BM_GenerateInteger(benchmark::State &state)
{
	srand(static_cast<unsigned int>(time(nullptr)));

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(rand());
	}
}
BENCHMARK(BM_GenerateInteger)->Arg(Repetitions);

static void BM_FastGenerateBoundedInteger(benchmark::State &state)
{
	srand(static_cast<unsigned int>(time(nullptr)));

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(50 + rand() % (100 - 50));
	}
}
BENCHMARK(BM_FastGenerateBoundedInteger)->Arg(Repetitions);

static void BM_FastGenerateReal(benchmark::State &state)
{
	srand(static_cast<unsigned int>(time(nullptr)));

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(static_cast<float>(rand() / static_cast<float>(RAND_MAX)));
	}
}
BENCHMARK(BM_FastGenerateReal)->Arg(Repetitions);

static void BM_FastGenerateBoundedReal(benchmark::State &state)
{
	srand(static_cast<unsigned int>(time(nullptr)));

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(5.0f + static_cast<float>(rand() / static_cast<float>(RAND_MAX)) * (10.0f - 5.0f));
	}
}
BENCHMARK(BM_FastGenerateBoundedReal)->Arg(Repetitions);

BENCHMARK_MAIN();
