#include "benchmark/benchmark.h"
#include <nctl/AllocManager.h>
#include <nctl/MallocAllocator.h>
#include <nctl/LinearAllocator.h>
#include <nctl/StackAllocator.h>
#include <nctl/PoolAllocator.h>
#include <nctl/FreeListAllocator.h>
#include <nctl/Array.h>

const unsigned int Capacity = 16;
const unsigned int BufferSize = 1024;
uint8_t buffer[BufferSize];

static void BM_Array_DefaultAllocator(benchmark::State &state)
{
	for (auto _ : state)
	{
		nctl::Array<unsigned int> array(state.range(0));
		benchmark::DoNotOptimize(array);
	}
}
BENCHMARK(BM_Array_DefaultAllocator)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_Array_LinearAllocator(benchmark::State &state)
{
	nctl::LinearAllocator linear(BufferSize, buffer);

	for (auto _ : state)
	{
		nctl::Array<unsigned int> array(state.range(0), linear);
		linear.clear();
		benchmark::DoNotOptimize(array);
	}
}
BENCHMARK(BM_Array_LinearAllocator)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

static void BM_Array_FreeListAllocator(benchmark::State &state)
{
	nctl::FreeListAllocator freelist(BufferSize, buffer);

	for (auto _ : state)
	{
		nctl::Array<unsigned int> array(state.range(0), freelist);
		benchmark::DoNotOptimize(array);
	}
}
BENCHMARK(BM_Array_LinearAllocator)->Arg(Capacity / 4)->Arg(Capacity / 2)->Arg(Capacity);

BENCHMARK_MAIN();
