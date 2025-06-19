#ifndef TEST_NONTRIVIAL_H
#define TEST_NONTRIVIAL_H

#include <cstdio>
#include <nctl/HashMap.h>
#include <nctl/HashSet.h>

namespace {

/// A non-trivial type class that tracks its special memeber functions
class NonTrivial
{
  public:
	NonTrivial()
	    : id_(++nextId_)
	{
		registerInstance("Default construct");
	}

	~NonTrivial()
	{
		printf("Destruct id=%u at %p\n", id_, this);

		if (liveInstances_.find(this) == nullptr)
			printf("Destructing an unknown or already destroyed object!\n");
		liveInstances_.remove(this);

		counter_--;
		destructions_++;
	}

	NonTrivial(const NonTrivial &other)
	    : id_(++nextId_)
	{
		registerInstance("Copy-construct", other.id_);
	}

	NonTrivial &operator=(const NonTrivial &other)
	{
		assignments_++;
		return *this;
	}

	NonTrivial(NonTrivial &&other)
	    : id_(++nextId_)
	{
		registerInstance("Move-construct", other.id_);
	}

	NonTrivial &operator=(NonTrivial &&other) noexcept
	{
		assignments_++;
		return *this;
	}

	unsigned int id() const { return id_; }
	static unsigned int counter() { return counter_; }
	static unsigned int constructions() { return constructions_; }
	static unsigned int destructions() { return destructions_; }
	static unsigned int assignments() { return assignments_; }
	static unsigned int liveInstances() { return liveInstances_.size(); }
	static unsigned int allInstanceAddresses() { return allInstanceAddresses_.size(); }

	static void resetCounters()
	{
		nextId_ = 0;
		counter_ = 0;
		constructions_ = 0;
		destructions_ = 0;
		assignments_ = 0;
		liveInstances_.clear();
		allInstanceAddresses_.clear();
	}

  private:
	unsigned int id_;
	static unsigned int nextId_;
	static unsigned int counter_;
	static unsigned int constructions_;
	static unsigned int destructions_;
	static unsigned int assignments_;
	static nctl::HashMap<const void *, unsigned int> liveInstances_;
	static nctl::HashSet<const void *> allInstanceAddresses_;

	void registerInstance(const char *what, unsigned int fromId)
	{
		// Detect construction on top of a live object
		ASSERT_TRUE(liveInstances_.find(this) == nullptr) << "Constructing over an existing live object!";
		liveInstances_.insert(this, id_);

		// Detect reuse of any address, without asserting and failing
		bool inserted = allInstanceAddresses_.insert(this);
		if (inserted == false)
			printf("This memory address was already used by another instance (even previously)\n");

		counter_++;
		constructions_++;

		if (fromId > 0)
			printf("%s id=%u from id=%u at %p\n", what, id_, fromId, this);
		else
			printf("%s id=%u at %p\n", what, id_, this);
	}

	void registerInstance(const char *what)
	{
		// Zero is a special id not used by any instance, that's why pre-increment is used with `++nextId_`
		registerInstance(what, 0);
	}
};

}

#endif
