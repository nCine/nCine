#ifndef CLASS_GRAIL_GENERATIONALPOOL
#define CLASS_GRAIL_GENERATIONALPOOL

#include <cstdint>
#include <nctl/Atomic.h>
#include <nctl/Array.h>

namespace ncine {
namespace grail {

/// A thread-safe pool of opaque handles with generation and an intrusive free list
class GenerationalPool
{
  public:
	static constexpr uint32_t Invalid = 0xFFFFFFFFu;

	explicit GenerationalPool(uint32_t capacity);

	uint32_t allocate();

	void free(uint32_t index);

	inline uint32_t generation(uint32_t index) const
	{
		return generations_[index].load(nctl::MemoryModel::RELAXED);
	}

	inline bool isAlive(uint32_t index, uint32_t generation) const
	{
		return (generations_[index].load(nctl::MemoryModel::RELAXED) == generation);
	}

	inline uint32_t size() const { return allocated_.load(nctl::MemoryModel::RELAXED); }

	/// Returns the maximum number of elements that can be allocated
	inline uint32_t capacity() const { return generations_.capacity(); }

  private:
	nctl::Array<nctl::AtomicU32> generations_;
	nctl::Array<nctl::AtomicU32> nextFree_;
	nctl::AtomicU32 freeListHead_;
	/// Number of allocated elements in the pool
	nctl::AtomicU32 allocated_;
};

} // namespace grail
} // namespace ncine

#endif
