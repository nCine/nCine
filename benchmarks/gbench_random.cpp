#include "benchmark/benchmark.h"
#include <ctime>
#include <ncine/Random.h>

namespace nc = ncine;
const unsigned int Repetitions = 1024;

static void BM_GenerateInteger(benchmark::State &state)
{
	nc::random().init(static_cast<uint64_t>(time(nullptr)), reinterpret_cast<intptr_t>(&nc::random()));

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(nc::random().integer());
	}
}
BENCHMARK(BM_GenerateInteger)->Arg(Repetitions);

static void BM_GenerateBoundedInteger(benchmark::State &state)
{
	nc::random().init(static_cast<uint64_t>(time(nullptr)), reinterpret_cast<intptr_t>(&nc::random()));

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(nc::random().integer(50, 100));
	}
}
BENCHMARK(BM_GenerateBoundedInteger)->Arg(Repetitions);

static void BM_FastGenerateBoundedInteger(benchmark::State &state)
{
	nc::random().init(static_cast<uint64_t>(time(nullptr)), reinterpret_cast<intptr_t>(&nc::random()));

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(nc::random().fastInteger(50, 100));
	}
}
BENCHMARK(BM_FastGenerateBoundedInteger)->Arg(Repetitions);

static void BM_GenerateReal(benchmark::State &state)
{
	nc::random().init(static_cast<uint64_t>(time(nullptr)), reinterpret_cast<intptr_t>(&nc::random()));

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(nc::random().real());
	}
}
BENCHMARK(BM_GenerateReal)->Arg(Repetitions);

static void BM_FastGenerateReal(benchmark::State &state)
{
	nc::random().init(static_cast<uint64_t>(time(nullptr)), reinterpret_cast<intptr_t>(&nc::random()));

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(nc::random().fastReal());
	}
}
BENCHMARK(BM_FastGenerateReal)->Arg(Repetitions);

static void BM_GenerateBoundedReal(benchmark::State &state)
{
	nc::random().init(static_cast<uint64_t>(time(nullptr)), reinterpret_cast<intptr_t>(&nc::random()));

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(nc::random().real(5.0f, 10.0f));
	}
}
BENCHMARK(BM_GenerateBoundedReal)->Arg(Repetitions);

static void BM_FastGenerateBoundedReal(benchmark::State &state)
{
	nc::random().init(static_cast<uint64_t>(time(nullptr)), reinterpret_cast<intptr_t>(&nc::random()));

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			benchmark::DoNotOptimize(nc::random().fastReal(5.0f, 10.0f));
	}
}
BENCHMARK(BM_FastGenerateBoundedReal)->Arg(Repetitions);

BENCHMARK_MAIN();
