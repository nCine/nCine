#include <ncine/common_macros.h>
#include <nctl/Atomic.h>

namespace nctl {

///////////////////////////////////////////////////////////
// Atomic32
///////////////////////////////////////////////////////////

int32_t Atomic32::load(MemoryModel memModel)
{
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return std::atomic_load_explicit(&value_, std::memory_order_relaxed);
		case MemoryModel::RELEASE:
			FATAL_MSG("Incompatible memory model");
			return 0;
		case MemoryModel::ACQUIRE:
			return std::atomic_load_explicit(&value_, std::memory_order_acquire);
		case MemoryModel::SEQ_CST:
		default:
			return std::atomic_load_explicit(&value_, std::memory_order_seq_cst);
	}
}

void Atomic32::store(int32_t value, MemoryModel memModel)
{
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			std::atomic_store_explicit(&value_, value, std::memory_order_relaxed);
			break;
		case MemoryModel::ACQUIRE:
			FATAL_MSG("Incompatible memory model");
			break;
		case MemoryModel::RELEASE:
			std::atomic_store_explicit(&value_, value, std::memory_order_release);
			break;
		case MemoryModel::SEQ_CST:
		default:
			std::atomic_store_explicit(&value_, value, std::memory_order_seq_cst);
			break;
	}
}

int32_t Atomic32::fetchAdd(int32_t value, MemoryModel memModel)
{
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return atomic_fetch_add_explicit(&value_, value, std::memory_order_relaxed);
		case MemoryModel::ACQUIRE:
			return atomic_fetch_add_explicit(&value_, value, std::memory_order_acquire);
		case MemoryModel::RELEASE:
			return atomic_fetch_add_explicit(&value_, value, std::memory_order_release);
		case MemoryModel::SEQ_CST:
		default:
			return atomic_fetch_add_explicit(&value_, value, std::memory_order_seq_cst);
	}
}

int32_t Atomic32::fetchSub(int32_t value, MemoryModel memModel)
{
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return atomic_fetch_sub_explicit(&value_, value, std::memory_order_relaxed);
		case MemoryModel::ACQUIRE:
			return atomic_fetch_sub_explicit(&value_, value, std::memory_order_acquire);
		case MemoryModel::RELEASE:
			return atomic_fetch_sub_explicit(&value_, value, std::memory_order_release);
		case MemoryModel::SEQ_CST:
		default:
			return atomic_fetch_sub_explicit(&value_, value, std::memory_order_seq_cst);
	}
}

bool Atomic32::cmpExchange(int32_t newValue, int32_t cmpValue, MemoryModel memModel)
{
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return std::atomic_compare_exchange_weak_explicit(&value_, &cmpValue, newValue, std::memory_order_relaxed, std::memory_order_relaxed);
		case MemoryModel::ACQUIRE:
			return std::atomic_compare_exchange_weak_explicit(&value_, &cmpValue, newValue, std::memory_order_acquire, std::memory_order_relaxed);
		case MemoryModel::RELEASE:
			return std::atomic_compare_exchange_weak_explicit(&value_, &cmpValue, newValue, std::memory_order_release, std::memory_order_relaxed);
		case MemoryModel::SEQ_CST:
		default:
			return std::atomic_compare_exchange_weak_explicit(&value_, &cmpValue, newValue, std::memory_order_seq_cst, std::memory_order_relaxed);
	}
}

///////////////////////////////////////////////////////////
// Atomic64
///////////////////////////////////////////////////////////

int64_t Atomic64::load(MemoryModel memModel)
{
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return std::atomic_load_explicit(&value_, std::memory_order_relaxed);
		case MemoryModel::RELEASE:
			FATAL_MSG("Incompatible memory model");
			return 0;
		case MemoryModel::ACQUIRE:
			return std::atomic_load_explicit(&value_, std::memory_order_acquire);
		case MemoryModel::SEQ_CST:
		default:
			return std::atomic_load_explicit(&value_, std::memory_order_seq_cst);
	}
}

void Atomic64::store(int64_t value, MemoryModel memModel)
{
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			std::atomic_store_explicit(&value_, value, std::memory_order_relaxed);
			break;
		case MemoryModel::ACQUIRE: FATAL_MSG("Incompatible memory model");
		case MemoryModel::RELEASE:
			std::atomic_store_explicit(&value_, value, std::memory_order_release);
			break;
		case MemoryModel::SEQ_CST:
		default:
			std::atomic_store_explicit(&value_, value, std::memory_order_seq_cst);
			break;
	}
}

int64_t Atomic64::fetchAdd(int64_t value, MemoryModel memModel)
{
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return atomic_fetch_add_explicit(&value_, value, std::memory_order_relaxed);
		case MemoryModel::ACQUIRE:
			return atomic_fetch_add_explicit(&value_, value, std::memory_order_acquire);
		case MemoryModel::RELEASE:
			return atomic_fetch_add_explicit(&value_, value, std::memory_order_release);
		case MemoryModel::SEQ_CST:
		default:
			return atomic_fetch_add_explicit(&value_, value, std::memory_order_seq_cst);
	}
}

int64_t Atomic64::fetchSub(int64_t value, MemoryModel memModel)
{
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return atomic_fetch_sub_explicit(&value_, value, std::memory_order_relaxed);
		case MemoryModel::ACQUIRE:
			return atomic_fetch_sub_explicit(&value_, value, std::memory_order_acquire);
		case MemoryModel::RELEASE:
			return atomic_fetch_sub_explicit(&value_, value, std::memory_order_release);
		case MemoryModel::SEQ_CST:
		default:
			return atomic_fetch_sub_explicit(&value_, value, std::memory_order_seq_cst);
	}
}

bool Atomic64::cmpExchange(int64_t newValue, int64_t cmpValue, MemoryModel memModel)
{
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return std::atomic_compare_exchange_weak_explicit(&value_, &cmpValue, newValue, std::memory_order_relaxed, std::memory_order_relaxed);
		case MemoryModel::ACQUIRE:
			return std::atomic_compare_exchange_weak_explicit(&value_, &cmpValue, newValue, std::memory_order_acquire, std::memory_order_relaxed);
		case MemoryModel::RELEASE:
			return std::atomic_compare_exchange_weak_explicit(&value_, &cmpValue, newValue, std::memory_order_release, std::memory_order_relaxed);
		case MemoryModel::SEQ_CST:
		default:
			return std::atomic_compare_exchange_weak_explicit(&value_, &cmpValue, newValue, std::memory_order_seq_cst, std::memory_order_relaxed);
	}
}

}
