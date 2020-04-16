#include "benchmark/benchmark.h"
#include <nctl/AllocManager.h>
#include <nctl/MallocAllocator.h>
#include <nctl/LinearAllocator.h>
#include <nctl/StackAllocator.h>
#include <nctl/PoolAllocator.h>
#include <nctl/FreeListAllocator.h>

const unsigned int BufferSize = 65536 * 1024 + 512;
uint16_t buffer[BufferSize];

const unsigned int Repetitions = 1024;
uint16_t allocSizes[Repetitions];

void setup()
{
	for (unsigned int i = 0; i < Repetitions; i++)
		allocSizes[i] = static_cast<uint16_t>((i << 8) + 255);
}

static void BM_RandomAllocations_new(benchmark::State &state)
{
	setup();
	uint8_t *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = static_cast<uint8_t *>(::operator new(allocSizes[i]));
		for (unsigned int i = 0; i < state.range(0); i++)
			::operator delete(ptrs[i]);
	}
}
BENCHMARK(BM_RandomAllocations_new)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

static void BM_RandomAllocations_malloc(benchmark::State &state)
{
	setup();
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = malloc(allocSizes[i]);
		for (unsigned int i = 0; i < state.range(0); i++)
			free(ptrs[i]);
	}
}
BENCHMARK(BM_RandomAllocations_malloc)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

static void BM_RandomAllocations_malloc_Reverse(benchmark::State &state)
{
	setup();
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = malloc(allocSizes[i]);
		for (unsigned int i = 0; i < state.range(0); i++)
			free(ptrs[state.range(0) - i - 1]);
	}
}
BENCHMARK(BM_RandomAllocations_malloc_Reverse)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

static void BM_RandomAllocations_MallocAllocator(benchmark::State &state)
{
	setup();
	nctl::MallocAllocator malloc;
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = malloc.allocate(allocSizes[i]);
		for (unsigned int i = 0; i < state.range(0); i++)
			malloc.deallocate(ptrs[i]);
	}
}
BENCHMARK(BM_RandomAllocations_MallocAllocator)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

static void BM_RandomAllocations_MallocAllocator_Reverse(benchmark::State &state)
{
	setup();
	nctl::MallocAllocator malloc;
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = malloc.allocate(allocSizes[i]);
		for (unsigned int i = 0; i < state.range(0); i++)
			malloc.deallocate(ptrs[state.range(0) - i - 1]);
	}
}
BENCHMARK(BM_RandomAllocations_MallocAllocator_Reverse)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

static void BM_RandomAllocations_LinearAllocator(benchmark::State &state)
{
	setup();
	nctl::LinearAllocator linear(BufferSize, buffer);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = linear.allocate(1024);
		linear.clear();
	}
}
BENCHMARK(BM_RandomAllocations_LinearAllocator)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

static void BM_RandomAllocations_StackAllocator(benchmark::State &state)
{
	setup();
	nctl::StackAllocator stack(BufferSize, buffer);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = stack.allocate(allocSizes[i]);
		for (unsigned int i = 0; i < state.range(0); i++)
			stack.deallocate(ptrs[state.range(0) - i - 1]);
	}
}
BENCHMARK(BM_RandomAllocations_StackAllocator)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

static void BM_RandomAllocations_PoolAllocator(benchmark::State &state)
{
	const unsigned int ElementSize = 256;
	nctl::PoolAllocator pool(ElementSize, BufferSize, buffer);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = pool.allocate(ElementSize);
		for (unsigned int i = 0; i < state.range(0); i++)
			pool.deallocate(ptrs[i]);
	}
}
BENCHMARK(BM_RandomAllocations_PoolAllocator)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

static void BM_RandomAllocations_FreeListAllocator(benchmark::State &state)
{
	setup();
	nctl::FreeListAllocator freelist(BufferSize, buffer);
	freelist.setDefragOnDeallocation(true);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = freelist.allocate(allocSizes[i]);
		for (unsigned int i = 0; i < state.range(0); i++)
			freelist.deallocate(ptrs[i]);
	}
}
BENCHMARK(BM_RandomAllocations_FreeListAllocator)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

static void BM_RandomAllocations_FreeListAllocator_Reverse(benchmark::State &state)
{
	setup();
	nctl::FreeListAllocator freelist(BufferSize, buffer);
	freelist.setDefragOnDeallocation(true);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = freelist.allocate(allocSizes[i]);
		for (unsigned int i = 0; i < state.range(0); i++)
			freelist.deallocate(ptrs[state.range(0) - i - 1]);
	}
}
BENCHMARK(BM_RandomAllocations_FreeListAllocator_Reverse)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

static void BM_RandomAllocations_FreeListAllocator_NoDefrag(benchmark::State &state)
{
	setup();
	nctl::FreeListAllocator freelist(BufferSize, buffer);
	freelist.setDefragOnDeallocation(false);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = freelist.allocate(allocSizes[i]);
		for (unsigned int i = 0; i < state.range(0); i++)
			freelist.deallocate(ptrs[i]);
	}
}
BENCHMARK(BM_RandomAllocations_FreeListAllocator_NoDefrag)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

static void BM_RandomAllocations_FreeListAllocator_NoDefrag_Reverse(benchmark::State &state)
{
	setup();
	nctl::FreeListAllocator freelist(BufferSize, buffer);
	freelist.setDefragOnDeallocation(false);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = freelist.allocate(allocSizes[i]);
		for (unsigned int i = 0; i < state.range(0); i++)
			freelist.deallocate(ptrs[state.range(0) - i - 1]);
	}
}
BENCHMARK(BM_RandomAllocations_FreeListAllocator_NoDefrag_Reverse)->Arg(Repetitions / 4)->Arg(Repetitions / 2)->Arg(Repetitions);

BENCHMARK_MAIN();
