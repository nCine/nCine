#include <ncine/common_macros.h>
#include <nctl/Atomic.h>
#include <ncine/common_windefines.h>
#include <windef.h>
#include <intrin.h>

namespace nctl {

///////////////////////////////////////////////////////////
// AtomicFences
///////////////////////////////////////////////////////////

void AtomicFences::threadFence(MemoryModel memModel)
{
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			// No fence needed
			break;
		case MemoryModel::ACQUIRE:
			_ReadBarrier(); // Compiler-level acquire fence
			break;
		case MemoryModel::RELEASE:
			_WriteBarrier(); // Compiler-level release fence
			break;
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			MemoryBarrier(); // Full memory fence (hardware + compiler)
			break;
	}
}

void AtomicFences::signalFence(MemoryModel memModel)
{
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			// No barrier needed
			break;
		case MemoryModel::ACQUIRE:
			_ReadBarrier(); // Prevent compiler reordering of reads
			break;
		case MemoryModel::RELEASE:
			_WriteBarrier(); // Prevent compiler reordering of writes
			break;
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			_ReadWriteBarrier(); // Prevent reordering of both
			break;
	}
}

///////////////////////////////////////////////////////////
// Atomic32
///////////////////////////////////////////////////////////

int32_t Atomic32::load(MemoryModel memModel) const
{
	const volatile long *ptr = reinterpret_cast<const volatile long *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return ReadNoFence(ptr);
		case MemoryModel::ACQUIRE:
		case MemoryModel::ACQ_REL:
			return ReadAcquire(ptr);
		case MemoryModel::RELEASE:
			FATAL_MSG("Incompatible memory model");
			return 0;
		case MemoryModel::SEQ_CST:
		default:
			return ReadAcquire(ptr);
	}
}

void Atomic32::store(int32_t value, MemoryModel memModel)
{
	volatile long *ptr = reinterpret_cast<volatile long *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			WriteNoFence(ptr, value);
			break;
		case MemoryModel::ACQUIRE:
			FATAL_MSG("Incompatible memory model");
			break;
		case MemoryModel::RELEASE:
		case MemoryModel::ACQ_REL:
			WriteRelease(ptr, value);
			break;
		case MemoryModel::SEQ_CST:
		default:
			WriteRelease(ptr, value);
			break;
	}
}

int32_t Atomic32::fetchAdd(int32_t value, MemoryModel memModel)
{
	volatile long *ptr = reinterpret_cast<volatile long *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedExchangeAddNoFence(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedExchangeAddAcquire(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedExchangeAddRelease(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedExchangeAdd(ptr, value);
	}
}

int32_t Atomic32::fetchSub(int32_t value, MemoryModel memModel)
{
	volatile long *ptr = reinterpret_cast<volatile long *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedExchangeAddNoFence(ptr, -value);
		case MemoryModel::ACQUIRE:
			return InterlockedExchangeAddAcquire(ptr, -value);
		case MemoryModel::RELEASE:
			return InterlockedExchangeAddRelease(ptr, -value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedExchangeAdd(ptr, -value);
	}
}

int32_t Atomic32::fetchAnd(int32_t value, MemoryModel memModel)
{
	volatile long *ptr = reinterpret_cast<volatile long *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedAndNoFence(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedAndAcquire(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedAndRelease(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedAnd(ptr, value);
	}
}

int32_t Atomic32::fetchOr(int32_t value, MemoryModel memModel)
{
	volatile long *ptr = reinterpret_cast<volatile long *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedOrNoFence(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedOrAcquire(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedOrRelease(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedOr(ptr, value);
	}
}

int32_t Atomic32::fetchXor(int32_t value, MemoryModel memModel)
{
	volatile long *ptr = reinterpret_cast<volatile long *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedXorNoFence(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedXorAcquire(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedXorRelease(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedXor(ptr, value);
	}
}

bool Atomic32::cmpExchange(int32_t &cmpValue, int32_t newValue, MemoryModel memModel)
{
	volatile long *ptr = reinterpret_cast<volatile long *>(&value_);
	long expected = cmpValue;
	long result;

	switch (memModel)
	{
		case MemoryModel::RELAXED:
			result = InterlockedCompareExchangeNoFence(ptr, newValue, expected);
			break;
		case MemoryModel::ACQUIRE:
			result = InterlockedCompareExchangeAcquire(ptr, newValue, expected);
			break;
		case MemoryModel::RELEASE:
			result = InterlockedCompareExchangeRelease(ptr, newValue, expected);
			break;
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			result = InterlockedCompareExchange(ptr, newValue, expected);
			break;
	}

	if (result == expected)
		return true;
	else
	{
		cmpValue = result;
		return false;
	}
}

///////////////////////////////////////////////////////////
// AtomicU32
///////////////////////////////////////////////////////////

uint32_t AtomicU32::load(MemoryModel memModel) const
{
	const volatile long *ptr = reinterpret_cast<const volatile long *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return ReadNoFence(ptr);
		case MemoryModel::ACQUIRE:
		case MemoryModel::ACQ_REL:
			return ReadAcquire(ptr);
		case MemoryModel::RELEASE:
			FATAL_MSG("Incompatible memory model");
			return 0;
		case MemoryModel::SEQ_CST:
		default:
			return ReadAcquire(ptr);
	}
}

void AtomicU32::store(uint32_t value, MemoryModel memModel)
{
	volatile long *ptr = reinterpret_cast<volatile long *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			WriteNoFence(ptr, value);
			break;
		case MemoryModel::ACQUIRE:
			FATAL_MSG("Incompatible memory model");
			break;
		case MemoryModel::RELEASE:
		case MemoryModel::ACQ_REL:
			WriteRelease(ptr, value);
			break;
		case MemoryModel::SEQ_CST:
		default:
			WriteRelease(ptr, value);
			break;
	}
}

uint32_t AtomicU32::fetchAdd(uint32_t value, MemoryModel memModel)
{
	volatile long *ptr = reinterpret_cast<volatile long *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedExchangeAddNoFence(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedExchangeAddAcquire(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedExchangeAddRelease(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedExchangeAdd(ptr, value);
	}
}

uint32_t AtomicU32::fetchSub(uint32_t value, MemoryModel memModel)
{
	volatile long *ptr = reinterpret_cast<volatile long *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedExchangeAddNoFence(ptr, -value);
		case MemoryModel::ACQUIRE:
			return InterlockedExchangeAddAcquire(ptr, -value);
		case MemoryModel::RELEASE:
			return InterlockedExchangeAddRelease(ptr, -value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedExchangeAdd(ptr, -value);
	}
}

uint32_t AtomicU32::fetchAnd(uint32_t value, MemoryModel memModel)
{
	volatile long *ptr = reinterpret_cast<volatile long *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedAndNoFence(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedAndAcquire(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedAndRelease(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedAnd(ptr, value);
	}
}

uint32_t AtomicU32::fetchOr(uint32_t value, MemoryModel memModel)
{
	volatile long *ptr = reinterpret_cast<volatile long *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedOrNoFence(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedOrAcquire(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedOrRelease(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedOr(ptr, value);
	}
}

uint32_t AtomicU32::fetchXor(uint32_t value, MemoryModel memModel)
{
	volatile long *ptr = reinterpret_cast<volatile long *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedXorNoFence(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedXorAcquire(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedXorRelease(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedXor(ptr, value);
	}
}

bool AtomicU32::cmpExchange(uint32_t &cmpValue, uint32_t newValue, MemoryModel memModel)
{
	volatile long *ptr = reinterpret_cast<volatile long *>(&value_);
	long expected = cmpValue;
	long result;

	switch (memModel)
	{
		case MemoryModel::RELAXED:
			result = InterlockedCompareExchangeNoFence(ptr, newValue, cmpValue);
			break;
		case MemoryModel::ACQUIRE:
			result = InterlockedCompareExchangeAcquire(ptr, newValue, cmpValue);
			break;
		case MemoryModel::RELEASE:
			result = InterlockedCompareExchangeRelease(ptr, newValue, cmpValue);
			break;
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			result = InterlockedCompareExchange(ptr, newValue, cmpValue);
			break;
	}

	if (result == expected)
		return true;
	else
	{
		cmpValue = result;
		return false;
	}
}

///////////////////////////////////////////////////////////
// Atomic64
///////////////////////////////////////////////////////////

int64_t Atomic64::load(MemoryModel memModel) const
{
	const volatile LONG64 *ptr = reinterpret_cast<const volatile LONG64 *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return ReadNoFence64(ptr);
		case MemoryModel::ACQUIRE:
		case MemoryModel::ACQ_REL:
			return ReadAcquire64(ptr);
		case MemoryModel::RELEASE:
			FATAL_MSG("Incompatible memory model");
			return 0;
		case MemoryModel::SEQ_CST:
		default:
			return ReadAcquire64(ptr);
	}
}

void Atomic64::store(int64_t value, MemoryModel memModel)
{
	volatile LONG64 *ptr = reinterpret_cast<volatile LONG64 *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			WriteNoFence64(ptr, value);
			break;
		case MemoryModel::ACQUIRE:
			FATAL_MSG("Incompatible memory model");
			break;
		case MemoryModel::RELEASE:
		case MemoryModel::ACQ_REL:
			WriteRelease64(ptr, value);
			break;
		case MemoryModel::SEQ_CST:
		default:
			WriteRelease64(ptr, value);
			break;
	}
}

int64_t Atomic64::fetchAdd(int64_t value, MemoryModel memModel)
{
	volatile LONG64 *ptr = reinterpret_cast<volatile LONG64 *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedExchangeAddNoFence64(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedExchangeAddAcquire64(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedExchangeAddRelease64(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedExchangeAdd64(ptr, value);
	}
}

int64_t Atomic64::fetchSub(int64_t value, MemoryModel memModel)
{
	volatile LONG64 *ptr = reinterpret_cast<volatile LONG64 *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedExchangeAddNoFence64(ptr, -value);
		case MemoryModel::ACQUIRE:
			return InterlockedExchangeAddAcquire64(ptr, -value);
		case MemoryModel::RELEASE:
			return InterlockedExchangeAddRelease64(ptr, -value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedExchangeAdd64(ptr, -value);
	}
}

int64_t Atomic64::fetchAnd(int64_t value, MemoryModel memModel)
{
	volatile LONG64 *ptr = reinterpret_cast<volatile LONG64 *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedAnd64NoFence(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedAnd64Acquire(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedAnd64Release(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedAnd64(ptr, value);
	}
}

int64_t Atomic64::fetchOr(int64_t value, MemoryModel memModel)
{
	volatile LONG64 *ptr = reinterpret_cast<volatile LONG64 *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedOr64NoFence(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedOr64Acquire(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedOr64Release(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedOr64(ptr, value);
	}
}

int64_t Atomic64::fetchXor(int64_t value, MemoryModel memModel)
{
	volatile LONG64 *ptr = reinterpret_cast<volatile LONG64 *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedXor64NoFence(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedXor64Acquire(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedXor64Release(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedXor64(ptr, value);
	}
}

bool Atomic64::cmpExchange(int64_t &cmpValue, int64_t newValue, MemoryModel memModel)
{
	volatile LONG64 *ptr = reinterpret_cast<volatile LONG64 *>(&value_);
	long expected = cmpValue;
	long result;

	switch (memModel)
	{
		case MemoryModel::RELAXED:
			result = InterlockedCompareExchangeNoFence64(ptr, newValue, cmpValue);
			break;
		case MemoryModel::ACQUIRE:
			result = InterlockedCompareExchangeAcquire64(ptr, newValue, cmpValue);
			break;
		case MemoryModel::RELEASE:
			result = InterlockedCompareExchangeRelease64(ptr, newValue, cmpValue);
			break;
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			result = InterlockedCompareExchange64(ptr, newValue, cmpValue);
			break;
	}

	if (result == expected)
		return true;
	else
	{
		cmpValue = result;
		return false;
	}
}

///////////////////////////////////////////////////////////
// AtomicU64
///////////////////////////////////////////////////////////

uint64_t AtomicU64::load(MemoryModel memModel) const
{
	const volatile LONG64 *ptr = reinterpret_cast<const volatile LONG64 *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return ReadNoFence64(ptr);
		case MemoryModel::ACQUIRE:
		case MemoryModel::ACQ_REL:
			return ReadAcquire64(ptr);
		case MemoryModel::RELEASE:
			FATAL_MSG("Incompatible memory model");
			return 0;
		case MemoryModel::SEQ_CST:
		default:
			return ReadAcquire64(ptr);
	}
}

void AtomicU64::store(uint64_t value, MemoryModel memModel)
{
	volatile LONG64 *ptr = reinterpret_cast<volatile LONG64 *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			WriteNoFence64(ptr, value);
			break;
		case MemoryModel::ACQUIRE:
			FATAL_MSG("Incompatible memory model");
			break;
		case MemoryModel::RELEASE:
		case MemoryModel::ACQ_REL:
			WriteRelease64(ptr, value);
			break;
		case MemoryModel::SEQ_CST:
		default:
			WriteRelease64(ptr, value);
			break;
	}
}

uint64_t AtomicU64::fetchAdd(uint64_t value, MemoryModel memModel)
{
	volatile LONG64 *ptr = reinterpret_cast<volatile LONG64 *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedExchangeAddNoFence64(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedExchangeAddAcquire64(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedExchangeAddRelease64(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedExchangeAdd64(ptr, value);
	}
}

uint64_t AtomicU64::fetchSub(uint64_t value, MemoryModel memModel)
{
	volatile LONG64 *ptr = reinterpret_cast<volatile LONG64 *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedExchangeAddNoFence64(ptr, -value);
		case MemoryModel::ACQUIRE:
			return InterlockedExchangeAddAcquire64(ptr, -value);
		case MemoryModel::RELEASE:
			return InterlockedExchangeAddRelease64(ptr, -value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedExchangeAdd64(ptr, -value);
	}
}

uint64_t AtomicU64::fetchAnd(uint64_t value, MemoryModel memModel)
{
	volatile LONG64 *ptr = reinterpret_cast<volatile LONG64 *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedAnd64NoFence(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedAnd64Acquire(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedAnd64Release(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedAnd64(ptr, value);
	}
}

uint64_t AtomicU64::fetchOr(uint64_t value, MemoryModel memModel)
{
	volatile LONG64 *ptr = reinterpret_cast<volatile LONG64 *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedOr64NoFence(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedOr64Acquire(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedOr64Release(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedOr64(ptr, value);
	}
}

uint64_t AtomicU64::fetchXor(uint64_t value, MemoryModel memModel)
{
	volatile LONG64 *ptr = reinterpret_cast<volatile LONG64 *>(&value_);
	switch (memModel)
	{
		case MemoryModel::RELAXED:
			return InterlockedXor64NoFence(ptr, value);
		case MemoryModel::ACQUIRE:
			return InterlockedXor64Acquire(ptr, value);
		case MemoryModel::RELEASE:
			return InterlockedXor64Release(ptr, value);
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			return InterlockedXor64(ptr, value);
	}
}

bool AtomicU64::cmpExchange(uint64_t &cmpValue, uint64_t newValue, MemoryModel memModel)
{
	volatile LONG64 *ptr = reinterpret_cast<volatile LONG64 *>(&value_);
	long expected = cmpValue;
	long result;

	switch (memModel)
	{
		case MemoryModel::RELAXED:
			result = InterlockedCompareExchangeNoFence64(ptr, newValue, cmpValue);
			break;
		case MemoryModel::ACQUIRE:
			result = InterlockedCompareExchangeAcquire64(ptr, newValue, cmpValue);
			break;
		case MemoryModel::RELEASE:
			result = InterlockedCompareExchangeRelease64(ptr, newValue, cmpValue);
			break;
		case MemoryModel::ACQ_REL:
		case MemoryModel::SEQ_CST:
		default:
			result = InterlockedCompareExchange64(ptr, newValue, cmpValue);
			break;
	}

	if (result == expected)
		return true;
	else
	{
		cmpValue = result;
		return false;
	}
}

}
