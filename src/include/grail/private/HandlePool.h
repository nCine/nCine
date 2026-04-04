#ifndef CLASS_GRAIL_HANDLEPOOL
#define CLASS_GRAIL_HANDLEPOOL

#include "grail/private/GenerationalPool.h"

namespace ncine {
namespace grail {

/// A wrapper around the generational pool for typed handles
template <class Handle>
class HandlePool
{
  public:
	explicit HandlePool(uint32_t capacity)
	    : genPool_(capacity) {}

	Handle create()
	{
		const uint32_t index = genPool_.allocate();
		ASSERT(index != GenerationalPool::Invalid);

		if (index == GenerationalPool::Invalid)
			return Handle::Invalid();

		const uint32_t generation = genPool_.generation(index);
		return Handle(index, generation);
	}

	void destroy(Handle handle)
	{
		if (handle.isValid() == false)
			return;

		const uint32_t index = handle.index();

		if (genPool_.isAlive(index, handle.generation()) == false)
			return;

		genPool_.free(index);
	}

	bool isAlive(Handle handle) const
	{
		if (handle.isValid() == false)
			return false;

		return genPool_.isAlive(handle.index(), handle.generation());
	}

  private:
	GenerationalPool genPool_;
};

} // namespace grail
} // namespace ncine

#endif
