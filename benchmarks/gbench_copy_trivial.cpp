#include "benchmark/benchmark.h"
#include <new>
#include <nctl/Array.h>

const unsigned int Capacity = 1024;

namespace {

struct Trivial
{
	int a;
	float b;
	double c;
	char d[8];
};

uint8_t bufferIntSrc[sizeof(Trivial) * Capacity];
uint8_t bufferIntDest[sizeof(Trivial) * Capacity];
Trivial *src = reinterpret_cast<Trivial *>(bufferIntSrc);
Trivial *dest = reinterpret_cast<Trivial *>(bufferIntDest);
using T = Trivial;

}

static void BM_Construct(benchmark::State &state)
{
	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			new (dest + i) T();
		benchmark::DoNotOptimize(dest);
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
		memcpy(dest, src, state.range(0) * sizeof(T));
		benchmark::DoNotOptimize(src);
		benchmark::DoNotOptimize(dest);
	}
}
BENCHMARK(BM_MemCopy)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_Destruct(benchmark::State &state)
{
	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			src[i].~T();
		benchmark::DoNotOptimize(src);
	}
}
BENCHMARK(BM_Destruct)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

BENCHMARK_MAIN();
