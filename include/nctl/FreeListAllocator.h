#ifndef CLASS_NCTL_FREELISTALLOCATOR
#define CLASS_NCTL_FREELISTALLOCATOR

#include <nctl/IAllocator.h>

namespace nctl {

/// A free list allocator
class DLL_PUBLIC FreeListAllocator : public IAllocator
{
  public:
	/// Block allocation strategy
	enum class FitStrategy
	{
		FIRST_FIT,
		BEST_FIT,
		WORST_FIT
	};

	struct Block
	{
		size_t size;
		Block *next;
	};

	FreeListAllocator();
	FreeListAllocator(size_t size, void *base)
	    : FreeListAllocator(size, base, true) {}
	FreeListAllocator(size_t size, void *base, bool defragOnDeallocation);
	~FreeListAllocator();

	void init(size_t size, void *base);
	/// Defragments free list blocks
	void defrag();
	inline const Block *freeBlock() const { return freeBlocks_; }

	/// Returns the allocation strategy
	inline FitStrategy fitStrategy() const { return fitStrategy_; }
	/// Sets the allocation strategy
	inline void setFitStrategy(FitStrategy fitStrategy) { fitStrategy_ = fitStrategy; }

	/// Returns the state of the defrag on deallocation flag
	inline bool defragOnDeallocation() const { return defragOnDeallocation_; }
	/// Sets the state of the defrag on deallocation flag
	/*! \note When the flag is true a defrag will automatically start after every deallocation */
	inline void setDefragOnDeallocation(bool value) { defragOnDeallocation_ = value; }

  private:
	struct Header
	{
		size_t size;
		uint8_t adjustment;
	};

	Block *freeBlocks_;
	FitStrategy fitStrategy_;
	bool defragOnDeallocation_;

	FreeListAllocator(const FreeListAllocator &) = delete;
	FreeListAllocator &operator=(const FreeListAllocator &) = delete;

	static void *allocateImpl(IAllocator *allocator, size_t size, uint8_t alignment);
	static void *reallocateImpl(IAllocator *allocator, void *ptr, size_t size, uint8_t alignment, size_t &oldSize);
	static void deallocateImpl(IAllocator *allocator, void *ptr);
};

}

#endif
