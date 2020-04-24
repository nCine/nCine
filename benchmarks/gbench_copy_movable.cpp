#include "benchmark/benchmark.h"
#include <new>
#include <nctl/Array.h>
#define TEST_WITH_NCTL
#include "test_movable.h"

const unsigned int Capacity = 1024;

namespace {

uint8_t bufferIntSrc[sizeof(Movable) * Capacity];
uint8_t bufferIntDest[sizeof(Movable) * Capacity];
Movable *src = reinterpret_cast<Movable *>(bufferIntSrc);
Movable *dest = reinterpret_cast<Movable *>(bufferIntDest);
using T = Movable;

}

static void BM_Construct(benchmark::State &state)
{
	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			new (dest + i) T(Movable::Construction::ALLOCATED);
		benchmark::DoNotOptimize(dest);

		state.PauseTiming();
		for (unsigned int i = 0; i < state.range(0); i++)
			dest[i].~T();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_Construct)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_CopyConstruct(benchmark::State &state)
{
	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			new (dest + i) T(src[i]);
		benchmark::DoNotOptimize(src);
		benchmark::DoNotOptimize(dest);

		state.PauseTiming();
		for (unsigned int i = 0; i < state.range(0); i++)
			dest[i].~T();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_CopyConstruct)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_MoveConstruct(benchmark::State &state)
{
	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			new (dest + i) T(nctl::move(src[i]));
		benchmark::DoNotOptimize(src);
		benchmark::DoNotOptimize(dest);

		state.PauseTiming();
		for (unsigned int i = 0; i < state.range(0); i++)
			dest[i].~T();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_MoveConstruct)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_CopyAssign(benchmark::State &state)
{
	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			dest[i] = src[i];
		benchmark::DoNotOptimize(src);
		benchmark::DoNotOptimize(dest);
	}
}
BENCHMARK(BM_CopyAssign)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_MoveAssign(benchmark::State &state)
{
	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			dest[i] = nctl::move(src[i]);
		benchmark::DoNotOptimize(src);
		benchmark::DoNotOptimize(dest);
	}
}
BENCHMARK(BM_MoveAssign)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_MemCopy(benchmark::State &state)
{
	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
		{
			new (dest + i) T(Movable::Construction::ALLOCATED);
			memcpy(dest[i].data(), src[i].data(), src[i].size() * sizeof(int));
		}
		benchmark::DoNotOptimize(src);
		benchmark::DoNotOptimize(dest);

		state.PauseTiming();
		for (unsigned int i = 0; i < state.range(0); i++)
			dest[i].~T();
		state.ResumeTiming();
	}
}
BENCHMARK(BM_MemCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_Destruct(benchmark::State &state)
{
	for (auto _ : state)
	{
		state.PauseTiming();
		for (unsigned int i = 0; i < state.range(0); i++)
			new (src + i) T(Movable::Construction::ALLOCATED);
		state.ResumeTiming();

		for (unsigned int i = 0; i < state.range(0); i++)
			src[i].~T();
		benchmark::DoNotOptimize(src);
	}
}
BENCHMARK(BM_Destruct)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

BENCHMARK_MAIN();
