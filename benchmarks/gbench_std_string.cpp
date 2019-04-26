#include "benchmark/benchmark.h"
#include <string>

const unsigned int Length = 256;

static void BM_StringCreation(benchmark::State &state)
{
	for (auto _ : state)
	{
		std::string string;
		string.reserve(state.range(0));
		benchmark::DoNotOptimize(string);
	}
}
BENCHMARK(BM_StringCreation)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

static void BM_StringCopy(benchmark::State &state)
{
	std::string initString;
	initString.reserve(state.range(0));
	for (unsigned int i = 0; i < state.range(0); i++)
		initString[i] = '0';
	std::string string;
	string.reserve(state.range(0));

	for (auto _ : state)
	{
		string = initString;
		benchmark::DoNotOptimize(string);

		state.PauseTiming();
		string.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_StringCopy)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

static void BM_StringAppend(benchmark::State &state)
{
	std::string string;

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			string.append("0");
			benchmark::DoNotOptimize(string);
		}

		state.PauseTiming();
		string.clear();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_StringAppend)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

static void BM_StringIterate(benchmark::State &state)
{
	std::string string;
	string.reserve(state.range(0));
	for (unsigned int i = 0; i < state.range(0); i++)
		string.append("0");

	for (auto _ : state)
	{
		for (char c : string)
		{
			char value = c;
			benchmark::DoNotOptimize(value);
		}
	}
}
BENCHMARK(BM_StringIterate)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

static void BM_StringClear(benchmark::State &state)
{
	std::string initString;
	initString.reserve(state.range(0));
	for (unsigned int i = 0; i < state.range(0); i++)
		initString[i] = '0';

	for (auto _ : state)
	{
		state.PauseTiming();
		std::string string(initString);
		state.ResumeTiming();

		string.clear();
	}
}
BENCHMARK(BM_StringClear)->Arg(Length / 4)->Arg(Length / 2)->Arg(Length);

BENCHMARK_MAIN();
