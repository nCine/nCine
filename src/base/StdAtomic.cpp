#include <ncine/common_macros.h>
#include <nctl/Atomic.h>

namespace nctl {

namespace {

	std::memory_order toStdMemoryModel(MemoryModel memModel)
	{
		switch (memModel)
		{
			case MemoryModel::RELAXED: return std::memory_order::memory_order_relaxed;
			case MemoryModel::ACQUIRE: return std::memory_order::memory_order_acquire;
			case MemoryModel::RELEASE: return std::memory_order::memory_order_release;
			case MemoryModel::ACQ_REL: return std::memory_order::memory_order_acq_rel;
			case MemoryModel::SEQ_CST: return std::memory_order::memory_order_seq_cst;
			default: return std::memory_order::memory_order_seq_cst;
		}
	}

}

///////////////////////////////////////////////////////////
// AtomicFences
///////////////////////////////////////////////////////////

void AtomicFences::threadFence(MemoryModel memModel)
{
	return std::atomic_thread_fence(toStdMemoryModel(memModel));
}

void AtomicFences::signalFence(MemoryModel memModel)
{
	return std::atomic_signal_fence(toStdMemoryModel(memModel));
}

///////////////////////////////////////////////////////////
// Atomic32
///////////////////////////////////////////////////////////

int32_t Atomic32::load(MemoryModel memModel) const
{
	if (memModel == MemoryModel::RELEASE)
		FATAL_MSG("Incompatible memory model");
	return value_.load(toStdMemoryModel(memModel));
}

void Atomic32::store(int32_t value, MemoryModel memModel)
{
	if (memModel == MemoryModel::ACQUIRE)
		FATAL_MSG("Incompatible memory model");
	value_.store(value, toStdMemoryModel(memModel));
}

int32_t Atomic32::fetchAdd(int32_t value, MemoryModel memModel)
{
	return value_.fetch_add(value, toStdMemoryModel(memModel));
}

int32_t Atomic32::fetchSub(int32_t value, MemoryModel memModel)
{
	return value_.fetch_sub(value, toStdMemoryModel(memModel));
}

int32_t Atomic32::fetchAnd(int32_t value, MemoryModel memModel)
{
	return value_.fetch_and(value, toStdMemoryModel(memModel));
}

int32_t Atomic32::fetchOr(int32_t value, MemoryModel memModel)
{
	return value_.fetch_or(value, toStdMemoryModel(memModel));
}

int32_t Atomic32::fetchXor(int32_t value, MemoryModel memModel)
{
	return value_.fetch_xor(value, toStdMemoryModel(memModel));
}

bool Atomic32::cmpExchange(int32_t &cmpValue, int32_t newValue, MemoryModel memModel)
{
	return value_.compare_exchange_weak(cmpValue, newValue, toStdMemoryModel(memModel), std::memory_order_relaxed);
}

///////////////////////////////////////////////////////////
// AtomicU32
///////////////////////////////////////////////////////////

uint32_t AtomicU32::load(MemoryModel memModel) const
{
	if (memModel == MemoryModel::RELEASE)
		FATAL_MSG("Incompatible memory model");
	return value_.load(toStdMemoryModel(memModel));
}

void AtomicU32::store(uint32_t value, MemoryModel memModel)
{
	if (memModel == MemoryModel::ACQUIRE)
		FATAL_MSG("Incompatible memory model");
	value_.store(value, toStdMemoryModel(memModel));
}

uint32_t AtomicU32::fetchAdd(uint32_t value, MemoryModel memModel)
{
	return value_.fetch_add(value, toStdMemoryModel(memModel));
}

uint32_t AtomicU32::fetchSub(uint32_t value, MemoryModel memModel)
{
	return value_.fetch_sub(value, toStdMemoryModel(memModel));
}

uint32_t AtomicU32::fetchAnd(uint32_t value, MemoryModel memModel)
{
	return value_.fetch_and(value, toStdMemoryModel(memModel));
}

uint32_t AtomicU32::fetchOr(uint32_t value, MemoryModel memModel)
{
	return value_.fetch_or(value, toStdMemoryModel(memModel));
}

uint32_t AtomicU32::fetchXor(uint32_t value, MemoryModel memModel)
{
	return value_.fetch_xor(value, toStdMemoryModel(memModel));
}

bool AtomicU32::cmpExchange(uint32_t &cmpValue, uint32_t newValue, MemoryModel memModel)
{
	return value_.compare_exchange_weak(cmpValue, newValue, toStdMemoryModel(memModel), std::memory_order_relaxed);
}

///////////////////////////////////////////////////////////
// Atomic64
///////////////////////////////////////////////////////////

int64_t Atomic64::load(MemoryModel memModel) const
{
	if (memModel == MemoryModel::RELEASE)
		FATAL_MSG("Incompatible memory model");
	return value_.load(toStdMemoryModel(memModel));
}

void Atomic64::store(int64_t value, MemoryModel memModel)
{
	if (memModel == MemoryModel::ACQUIRE)
		FATAL_MSG("Incompatible memory model");
	value_.store(value, toStdMemoryModel(memModel));
}

int64_t Atomic64::fetchAdd(int64_t value, MemoryModel memModel)
{
	return value_.fetch_add(value, toStdMemoryModel(memModel));
}

int64_t Atomic64::fetchSub(int64_t value, MemoryModel memModel)
{
	return value_.fetch_sub(value, toStdMemoryModel(memModel));
}

int64_t Atomic64::fetchAnd(int64_t value, MemoryModel memModel)
{
	return value_.fetch_and(value, toStdMemoryModel(memModel));
}

int64_t Atomic64::fetchOr(int64_t value, MemoryModel memModel)
{
	return value_.fetch_or(value, toStdMemoryModel(memModel));
}

int64_t Atomic64::fetchXor(int64_t value, MemoryModel memModel)
{
	return value_.fetch_xor(value, toStdMemoryModel(memModel));
}

bool Atomic64::cmpExchange(int64_t &cmpValue, int64_t newValue, MemoryModel memModel)
{
	return value_.compare_exchange_weak(cmpValue, newValue, toStdMemoryModel(memModel), std::memory_order_relaxed);
}

///////////////////////////////////////////////////////////
// AtomicU64
///////////////////////////////////////////////////////////

uint64_t AtomicU64::load(MemoryModel memModel) const
{
	if (memModel == MemoryModel::RELEASE)
		FATAL_MSG("Incompatible memory model");
	return value_.load(toStdMemoryModel(memModel));
}

void AtomicU64::store(uint64_t value, MemoryModel memModel)
{
	if (memModel == MemoryModel::ACQUIRE)
		FATAL_MSG("Incompatible memory model");
	value_.store(value, toStdMemoryModel(memModel));
}

uint64_t AtomicU64::fetchAdd(uint64_t value, MemoryModel memModel)
{
	return value_.fetch_add(value, toStdMemoryModel(memModel));
}

uint64_t AtomicU64::fetchSub(uint64_t value, MemoryModel memModel)
{
	return value_.fetch_sub(value, toStdMemoryModel(memModel));
}

uint64_t AtomicU64::fetchAnd(uint64_t value, MemoryModel memModel)
{
	return value_.fetch_and(value, toStdMemoryModel(memModel));
}

uint64_t AtomicU64::fetchOr(uint64_t value, MemoryModel memModel)
{
	return value_.fetch_or(value, toStdMemoryModel(memModel));
}

uint64_t AtomicU64::fetchXor(uint64_t value, MemoryModel memModel)
{
	return value_.fetch_xor(value, toStdMemoryModel(memModel));
}

bool AtomicU64::cmpExchange(uint64_t &cmpValue, uint64_t newValue, MemoryModel memModel)
{
	return value_.compare_exchange_weak(cmpValue, newValue, toStdMemoryModel(memModel), std::memory_order_relaxed);
}

}
