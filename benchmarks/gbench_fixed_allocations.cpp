#include "benchmark/benchmark.h"
#include <nctl/AllocManager.h>
#include <nctl/MallocAllocator.h>
#include <nctl/LinearAllocator.h>
#include <nctl/StackAllocator.h>
#include <nctl/PoolAllocator.h>
#include <nctl/FreeListAllocator.h>

const unsigned int BufferSize = (65536 + 4) * 1024;
uint8_t buffer[BufferSize];

const unsigned int Repetitions = 1024;

static void BM_FixedAllocations_new(benchmark::State &state)
{
	uint8_t *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = static_cast<uint8_t *>(::operator new(state.range(1)));
		for (unsigned int i = 0; i < state.range(0); i++)
			::operator delete(ptrs[i]);
	}
}
BENCHMARK(BM_FixedAllocations_new)->Args({ Repetitions / 4, 1024 })->Args({ Repetitions / 2, 1024 })->Args({ Repetitions, 1024 })
                                  ->Args({ Repetitions / 4, 4096 })->Args({ Repetitions / 2, 4096 })->Args({ Repetitions, 4096 })
                                  ->Args({ Repetitions / 4, 65536 })->Args({ Repetitions / 2, 65536 })->Args({ Repetitions, 65536 });

static void BM_FixedAllocations_malloc(benchmark::State &state)
{
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = malloc(state.range(1));
		for (unsigned int i = 0; i < state.range(0); i++)
			free(ptrs[i]);
	}
}
BENCHMARK(BM_FixedAllocations_malloc)->Args({ Repetitions / 4, 1024 })->Args({ Repetitions / 2, 1024 })->Args({ Repetitions, 1024 })
                                     ->Args({ Repetitions / 4, 4096 })->Args({ Repetitions / 2, 4096 })->Args({ Repetitions, 4096 })
                                     ->Args({ Repetitions / 4, 65536 })->Args({ Repetitions / 2, 65536 })->Args({ Repetitions, 65536 });

static void BM_FixedAllocations_malloc_Reverse(benchmark::State &state)
{
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = malloc(state.range(1));
		for (unsigned int i = 0; i < state.range(0); i++)
			free(ptrs[state.range(0) - i - 1]);
	}
}
BENCHMARK(BM_FixedAllocations_malloc_Reverse)->Args({ Repetitions / 4, 1024 })->Args({ Repetitions / 2, 1024 })->Args({ Repetitions, 1024 })
                                             ->Args({ Repetitions / 4, 4096 })->Args({ Repetitions / 2, 4096 })->Args({ Repetitions, 4096 })
                                             ->Args({ Repetitions / 4, 65536 })->Args({ Repetitions / 2, 65536 })->Args({ Repetitions, 65536 });

static void BM_FixedAllocations_MallocAllocator(benchmark::State &state)
{
	nctl::MallocAllocator malloc;
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = malloc.allocate(state.range(1));
		for (unsigned int i = 0; i < state.range(0); i++)
			malloc.deallocate(ptrs[i]);
	}
}
BENCHMARK(BM_FixedAllocations_MallocAllocator)->Args({ Repetitions / 4, 1024 })->Args({ Repetitions / 2, 1024 })->Args({ Repetitions, 1024 })
                                              ->Args({ Repetitions / 4, 4096 })->Args({ Repetitions / 2, 4096 })->Args({ Repetitions, 4096 })
                                              ->Args({ Repetitions / 4, 65536 })->Args({ Repetitions / 2, 65536 })->Args({ Repetitions, 65536 });

static void BM_FixedAllocations_MallocAllocator_Reverse(benchmark::State &state)
{
	nctl::MallocAllocator malloc;
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = malloc.allocate(state.range(1));
		for (unsigned int i = 0; i < state.range(0); i++)
			malloc.deallocate(ptrs[state.range(0) - i - 1]);
	}
}
BENCHMARK(BM_FixedAllocations_MallocAllocator_Reverse)->Args({ Repetitions / 4, 1024 })->Args({ Repetitions / 2, 1024 })->Args({ Repetitions, 1024 })
                                                      ->Args({ Repetitions / 4, 4096 })->Args({ Repetitions / 2, 4096 })->Args({ Repetitions, 4096 })
                                                      ->Args({ Repetitions / 4, 65536 })->Args({ Repetitions / 2, 65536 })->Args({ Repetitions, 65536 });

static void BM_FixedAllocations_LinearAllocator(benchmark::State &state)
{
	nctl::LinearAllocator linear(BufferSize, buffer);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = linear.allocate(state.range(1));
		linear.clear();
	}
}
BENCHMARK(BM_FixedAllocations_LinearAllocator)->Args({ Repetitions / 4, 1024 })->Args({ Repetitions / 2, 1024 })->Args({ Repetitions, 1024 })
                                              ->Args({ Repetitions / 4, 4096 })->Args({ Repetitions / 2, 4096 })->Args({ Repetitions, 4096 })
                                              ->Args({ Repetitions / 4, 65536 })->Args({ Repetitions / 2, 65536 })->Args({ Repetitions, 65536 });

static void BM_FixedAllocations_StackAllocator(benchmark::State &state)
{
	nctl::StackAllocator stack(BufferSize, buffer);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = stack.allocate(state.range(1));
		for (unsigned int i = 0; i < state.range(0); i++)
			stack.deallocate(ptrs[state.range(0) - i - 1]);
	}
}
BENCHMARK(BM_FixedAllocations_StackAllocator)->Args({ Repetitions / 4, 1024 })->Args({ Repetitions / 2, 1024 })->Args({ Repetitions, 1024 })
                                             ->Args({ Repetitions / 4, 4096 })->Args({ Repetitions / 2, 4096 })->Args({ Repetitions, 4096 })
                                             ->Args({ Repetitions / 4, 65536 })->Args({ Repetitions / 2, 65536 })->Args({ Repetitions, 65536 });

static void BM_FixedAllocations_PoolAllocator(benchmark::State &state)
{
	nctl::PoolAllocator pool(state.range(1), BufferSize, buffer);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = pool.allocate(state.range(1));
		for (unsigned int i = 0; i < state.range(0); i++)
			pool.deallocate(ptrs[i]);
	}
}
BENCHMARK(BM_FixedAllocations_PoolAllocator)->Args({ Repetitions / 4, 1024 })->Args({ Repetitions / 2, 1024 })->Args({ Repetitions, 1024 })
                                            ->Args({ Repetitions / 4, 4096 })->Args({ Repetitions / 2, 4096 })->Args({ Repetitions, 4096 })
                                            ->Args({ Repetitions / 4, 65536 })->Args({ Repetitions / 2, 65536 })->Args({ Repetitions, 65536 });

static void BM_FixedAllocations_PoolAllocator_Reverse(benchmark::State &state)
{
	nctl::PoolAllocator pool(state.range(1), BufferSize, buffer);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = pool.allocate(state.range(1));
		for (unsigned int i = 0; i < state.range(0); i++)
			pool.deallocate(ptrs[state.range(0) - i - 1]);
	}
}
BENCHMARK(BM_FixedAllocations_PoolAllocator_Reverse)->Args({ Repetitions / 4, 1024 })->Args({ Repetitions / 2, 1024 })->Args({ Repetitions, 1024 })
                                                    ->Args({ Repetitions / 4, 4096 })->Args({ Repetitions / 2, 4096 })->Args({ Repetitions, 4096 })
                                                    ->Args({ Repetitions / 4, 65536 })->Args({ Repetitions / 2, 65536 })->Args({ Repetitions, 65536 });

static void BM_FixedAllocations_FreeListAllocator(benchmark::State &state)
{
	nctl::FreeListAllocator freelist(BufferSize, buffer);
	freelist.setDefragOnDeallocation(true);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = freelist.allocate(state.range(1));
		for (unsigned int i = 0; i < state.range(0); i++)
			freelist.deallocate(ptrs[i]);
	}
}
BENCHMARK(BM_FixedAllocations_FreeListAllocator)->Args({ Repetitions / 4, 1024 })->Args({ Repetitions / 2, 1024 })->Args({ Repetitions, 1024 })
                                                ->Args({ Repetitions / 4, 4096 })->Args({ Repetitions / 2, 4096 })->Args({ Repetitions, 4096 })
                                                ->Args({ Repetitions / 4, 65536 })->Args({ Repetitions / 2, 65536 })->Args({ Repetitions, 65536 });

static void BM_FixedAllocations_FreeListAllocator_Reverse(benchmark::State &state)
{
	nctl::FreeListAllocator freelist(BufferSize, buffer);
	freelist.setDefragOnDeallocation(true);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = freelist.allocate(state.range(1));
		for (unsigned int i = 0; i < state.range(0); i++)
			freelist.deallocate(ptrs[state.range(0) - i - 1]);
	}
}
BENCHMARK(BM_FixedAllocations_FreeListAllocator_Reverse)->Args({ Repetitions / 4, 1024 })->Args({ Repetitions / 2, 1024 })->Args({ Repetitions, 1024 })
                                                        ->Args({ Repetitions / 4, 4096 })->Args({ Repetitions / 2, 4096 })->Args({ Repetitions, 4096 })
                                                        ->Args({ Repetitions / 4, 65536 })->Args({ Repetitions / 2, 65536 })->Args({ Repetitions, 65536 });

static void BM_FixedAllocations_FreeListAllocator_NoDefrag(benchmark::State &state)
{
	nctl::FreeListAllocator freelist(BufferSize, buffer);
	freelist.setDefragOnDeallocation(false);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = freelist.allocate(state.range(1));
		for (unsigned int i = 0; i < state.range(0); i++)
			freelist.deallocate(ptrs[i]);
	}
}
BENCHMARK(BM_FixedAllocations_FreeListAllocator_NoDefrag)->Args({ Repetitions / 4, 1024 })->Args({ Repetitions / 2, 1024 })->Args({ Repetitions, 1024 })
                                                         ->Args({ Repetitions / 4, 4096 })->Args({ Repetitions / 2, 4096 })->Args({ Repetitions, 4096 })
                                                         ->Args({ Repetitions / 4, 65536 })->Args({ Repetitions / 2, 65536 })->Args({ Repetitions, 65536 });

static void BM_FixedAllocations_FreeListAllocator_Reverse_NoDefrag(benchmark::State &state)
{
	nctl::FreeListAllocator freelist(BufferSize, buffer);
	freelist.setDefragOnDeallocation(false);
	void *ptrs[Repetitions];

	for (auto _ : state)
	{
		for (unsigned int i = 0; i < state.range(0); i++)
			ptrs[i] = freelist.allocate(state.range(1));
		for (unsigned int i = 0; i < state.range(0); i++)
			freelist.deallocate(ptrs[state.range(0) - i - 1]);
	}
}
BENCHMARK(BM_FixedAllocations_FreeListAllocator_Reverse_NoDefrag)->Args({ Repetitions / 4, 1024 })->Args({ Repetitions / 2, 1024 })->Args({ Repetitions, 1024 })
                                                                 ->Args({ Repetitions / 4, 4096 })->Args({ Repetitions / 2, 4096 })->Args({ Repetitions, 4096 })
                                                                 ->Args({ Repetitions / 4, 65536 })->Args({ Repetitions / 2, 65536 })->Args({ Repetitions, 65536 });

BENCHMARK_MAIN();
