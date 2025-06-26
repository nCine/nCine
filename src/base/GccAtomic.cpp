#include <ncine/common_macros.h>
#include <nctl/Atomic.h>

namespace nctl {

namespace {

	int toNativeMemoryModel(MemoryModel memModel)
	{
		switch (memModel)
		{
			case MemoryModel::RELAXED: return __ATOMIC_RELAXED;
			case MemoryModel::ACQUIRE: return __ATOMIC_ACQUIRE;
			case MemoryModel::RELEASE: return __ATOMIC_RELEASE;
			case MemoryModel::ACQ_REL: return __ATOMIC_ACQ_REL;
			case MemoryModel::SEQ_CST: return __ATOMIC_SEQ_CST;
			default: return __ATOMIC_SEQ_CST;
		}
	}

}

///////////////////////////////////////////////////////////
// AtomicFences
///////////////////////////////////////////////////////////

void AtomicFences::threadFence(MemoryModel memModel)
{
	if (memModel == MemoryModel::RELEASE)
		FATAL_MSG("Incompatible memory model");
	__atomic_thread_fence(toNativeMemoryModel(memModel));
}

void AtomicFences::signalFence(MemoryModel memModel)
{
	if (memModel == MemoryModel::RELEASE)
		FATAL_MSG("Incompatible memory model");
	__atomic_signal_fence(toNativeMemoryModel(memModel));
}

///////////////////////////////////////////////////////////
// Atomic32
///////////////////////////////////////////////////////////

int32_t Atomic32::load(MemoryModel memModel) const
{
	if (memModel == MemoryModel::RELEASE)
		FATAL_MSG("Incompatible memory model");
	return __atomic_load_n(&value_, toNativeMemoryModel(memModel));
}

void Atomic32::store(int32_t value, MemoryModel memModel)
{
	if (memModel == MemoryModel::ACQUIRE)
		FATAL_MSG("Incompatible memory model");
	__atomic_store_n(&value_, value, toNativeMemoryModel(memModel));
}

int32_t Atomic32::fetchAdd(int32_t value, MemoryModel memModel)
{
	return __atomic_fetch_add(&value_, value, toNativeMemoryModel(memModel));
}

int32_t Atomic32::fetchSub(int32_t value, MemoryModel memModel)
{
	return __atomic_fetch_sub(&value_, value, toNativeMemoryModel(memModel));
}

int32_t Atomic32::fetchAnd(int32_t value, MemoryModel memModel)
{
	return __atomic_fetch_and(&value_, value, toNativeMemoryModel(memModel));
}

int32_t Atomic32::fetchOr(int32_t value, MemoryModel memModel)
{
	return __atomic_fetch_or(&value_, value, toNativeMemoryModel(memModel));
}

int32_t Atomic32::fetchXor(int32_t value, MemoryModel memModel)
{
	return __atomic_fetch_xor(&value_, value, toNativeMemoryModel(memModel));
}

bool Atomic32::cmpExchange(int32_t &cmpValue, int32_t newValue, MemoryModel memModel)
{
	return __atomic_compare_exchange_n(&value_, &cmpValue, newValue, false, toNativeMemoryModel(memModel), __ATOMIC_RELAXED);
}

///////////////////////////////////////////////////////////
// AtomicU32
///////////////////////////////////////////////////////////

uint32_t AtomicU32::load(MemoryModel memModel) const
{
	if (memModel == MemoryModel::RELEASE)
		FATAL_MSG("Incompatible memory model");
	return __atomic_load_n(&value_, toNativeMemoryModel(memModel));
}

void AtomicU32::store(uint32_t value, MemoryModel memModel)
{
	if (memModel == MemoryModel::ACQUIRE)
		FATAL_MSG("Incompatible memory model");
	__atomic_store_n(&value_, value, toNativeMemoryModel(memModel));
}

uint32_t AtomicU32::fetchAdd(uint32_t value, MemoryModel memModel)
{
	return __atomic_fetch_add(&value_, value, toNativeMemoryModel(memModel));
}

uint32_t AtomicU32::fetchSub(uint32_t value, MemoryModel memModel)
{
	return __atomic_fetch_sub(&value_, value, toNativeMemoryModel(memModel));
}

uint32_t AtomicU32::fetchAnd(uint32_t value, MemoryModel memModel)
{
	return __atomic_fetch_and(&value_, value, toNativeMemoryModel(memModel));
}

uint32_t AtomicU32::fetchOr(uint32_t value, MemoryModel memModel)
{
	return __atomic_fetch_or(&value_, value, toNativeMemoryModel(memModel));
}

uint32_t AtomicU32::fetchXor(uint32_t value, MemoryModel memModel)
{
	return __atomic_fetch_xor(&value_, value, toNativeMemoryModel(memModel));
}

bool AtomicU32::cmpExchange(uint32_t &cmpValue, uint32_t newValue, MemoryModel memModel)
{
	return __atomic_compare_exchange_n(&value_, &cmpValue, newValue, false, toNativeMemoryModel(memModel), __ATOMIC_RELAXED);
}

///////////////////////////////////////////////////////////
// Atomic64
///////////////////////////////////////////////////////////

int64_t Atomic64::load(MemoryModel memModel) const
{
	if (memModel == MemoryModel::RELEASE)
		FATAL_MSG("Incompatible memory model");
	return __atomic_load_n(&value_, toNativeMemoryModel(memModel));
}

void Atomic64::store(int64_t value, MemoryModel memModel)
{
	if (memModel == MemoryModel::ACQUIRE)
		FATAL_MSG("Incompatible memory model");
	__atomic_store_n(&value_, value, toNativeMemoryModel(memModel));
}

int64_t Atomic64::fetchAdd(int64_t value, MemoryModel memModel)
{
	return __atomic_fetch_add(&value_, value, toNativeMemoryModel(memModel));
}

int64_t Atomic64::fetchSub(int64_t value, MemoryModel memModel)
{
	return __atomic_fetch_sub(&value_, value, toNativeMemoryModel(memModel));
}

int64_t Atomic64::fetchAnd(int64_t value, MemoryModel memModel)
{
	return __atomic_fetch_and(&value_, value, toNativeMemoryModel(memModel));
}

int64_t Atomic64::fetchOr(int64_t value, MemoryModel memModel)
{
	return __atomic_fetch_or(&value_, value, toNativeMemoryModel(memModel));
}

int64_t Atomic64::fetchXor(int64_t value, MemoryModel memModel)
{
	return __atomic_fetch_xor(&value_, value, toNativeMemoryModel(memModel));
}

bool Atomic64::cmpExchange(int64_t &cmpValue, int64_t newValue, MemoryModel memModel)
{
	return __atomic_compare_exchange_n(&value_, &cmpValue, newValue, false, toNativeMemoryModel(memModel), __ATOMIC_RELAXED);
}

///////////////////////////////////////////////////////////
// AtomicU64
///////////////////////////////////////////////////////////

uint64_t AtomicU64::load(MemoryModel memModel) const
{
	if (memModel == MemoryModel::RELEASE)
		FATAL_MSG("Incompatible memory model");
	return __atomic_load_n(&value_, toNativeMemoryModel(memModel));
}

void AtomicU64::store(uint64_t value, MemoryModel memModel)
{
	if (memModel == MemoryModel::ACQUIRE)
		FATAL_MSG("Incompatible memory model");
	__atomic_store_n(&value_, value, toNativeMemoryModel(memModel));
}

uint64_t AtomicU64::fetchAdd(uint64_t value, MemoryModel memModel)
{
	return __atomic_fetch_add(&value_, value, toNativeMemoryModel(memModel));
}

uint64_t AtomicU64::fetchSub(uint64_t value, MemoryModel memModel)
{
	return __atomic_fetch_sub(&value_, value, toNativeMemoryModel(memModel));
}

uint64_t AtomicU64::fetchAnd(uint64_t value, MemoryModel memModel)
{
	return __atomic_fetch_and(&value_, value, toNativeMemoryModel(memModel));
}

uint64_t AtomicU64::fetchOr(uint64_t value, MemoryModel memModel)
{
	return __atomic_fetch_or(&value_, value, toNativeMemoryModel(memModel));
}

uint64_t AtomicU64::fetchXor(uint64_t value, MemoryModel memModel)
{
	return __atomic_fetch_xor(&value_, value, toNativeMemoryModel(memModel));
}

bool AtomicU64::cmpExchange(uint64_t &cmpValue, uint64_t newValue, MemoryModel memModel)
{
	return __atomic_compare_exchange_n(&value_, &cmpValue, newValue, false, toNativeMemoryModel(memModel), __ATOMIC_RELAXED);
}

}
