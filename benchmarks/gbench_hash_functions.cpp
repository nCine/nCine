#include "benchmark/benchmark.h"
#include <nctl/HashFunctions.h>
#include <nctl/String.h>

const unsigned int Capacity = 4096;

static void FillBuffer(char *buffer, size_t size)
{
	for (size_t i = 0; i < size; i++)
		buffer[i] = static_cast<char>(i * 131);
}

static void BM_FastHash64(benchmark::State &state)
{
	const size_t size = state.range(0);
	char buffer[Capacity];
	FillBuffer(buffer, size);

	for (auto _ : state)
	{
		uint64_t hash = nctl::fasthash64(buffer, size, 0x01000193811C9DC5ULL);
		benchmark::DoNotOptimize(hash);
	}

	state.SetBytesProcessed(state.iterations() * size);
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_FastHash64)->Arg(Capacity / 16)->Arg(Capacity / 8)->Arg(Capacity / 4)->Arg(Capacity);

static void BM_FastHash32(benchmark::State &state)
{
	const size_t size = state.range(0);
	char buffer[Capacity];
	FillBuffer(buffer, size);

	for (auto _ : state)
	{
		uint32_t hash = nctl::fasthash32(buffer, size, 0x811C9DC5);
		benchmark::DoNotOptimize(hash);
	}

	state.SetBytesProcessed(state.iterations() * size);
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_FastHash32)->Arg(Capacity / 16)->Arg(Capacity / 8)->Arg(Capacity / 4)->Arg(Capacity);

template <class HashFunc>
static void BM_StringHash(benchmark::State &state)
{
	const size_t size = state.range(0);

	nctl::String str(size);
	str.setLength(size);

	for (size_t i = 0; i < size; i++)
		str[i] = 'a' + (i % 26);
	benchmark::DoNotOptimize(str.data());

	HashFunc hashFunc;
	for (auto _ : state)
	{
		uint32_t hash = hashFunc(str);
		benchmark::DoNotOptimize(hash);
	}

	state.SetBytesProcessed(state.iterations() * size);
	state.SetItemsProcessed(state.iterations());
}

BENCHMARK_TEMPLATE(BM_StringHash, nctl::deprecated::SaxHashFunc<nctl::String>)->Arg(Capacity / 16)->Arg(Capacity / 8)->Arg(Capacity / 4)->Arg(Capacity);
BENCHMARK_TEMPLATE(BM_StringHash, nctl::deprecated::JenkinsHashFunc<nctl::String>)->Arg(Capacity / 16)->Arg(Capacity / 8)->Arg(Capacity / 4)->Arg(Capacity);
BENCHMARK_TEMPLATE(BM_StringHash, nctl::deprecated::FNV1aHashFunc<nctl::String>)->Arg(Capacity / 16)->Arg(Capacity / 8)->Arg(Capacity / 4)->Arg(Capacity);
BENCHMARK_TEMPLATE(BM_StringHash, nctl::FastHashFunc<nctl::String>)->Arg(Capacity / 16)->Arg(Capacity / 8)->Arg(Capacity / 4)->Arg(Capacity);

BENCHMARK_MAIN();
