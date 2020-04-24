#ifndef TEST_REFCOUNTED_H
#define TEST_REFCOUNTED_H

#define TEST_MOVABLE_ONLY (0)

namespace {

class RefCounted
{
  public:
	RefCounted()
	{
		constructions_++;
		counter_++;
		initialized_ = true;
	}
	~RefCounted()
	{
		destructions_++;
		counter_--;
		initialized_ = false;
	}

	RefCounted(const RefCounted &other)
	{
		copyConstructions_++;
		counter_++;
		initialized_ = true;
	}
	RefCounted(RefCounted &&other)
	{
		moveConstructions_++;
		counter_++;
		initialized_ = true;
	}

	RefCounted &operator=(const RefCounted &other)
	{
		assignments_++;
		if (!initialized_)
			counter_++;
		return *this;
	}
	RefCounted &operator=(RefCounted &&other)
	{
		moveAssignments_++;
		if (!initialized_)
			counter_++;
		return *this;
	}

	inline bool operator==(const RefCounted &other) const { return true; }

	inline static int counter() { return counter_; }

	inline static int constructions() { return constructions_; }
	inline static int destructions() { return destructions_; }
	inline static int copyConstructions() { return copyConstructions_; }
	inline static int moveConstructions() { return moveConstructions_; }
	inline static int assignments() { return assignments_; }
	inline static int moveAssignments() { return moveAssignments_; }

	static void reset()
	{
		// Reset everything except the reference counter
		constructions_ = 0;
		destructions_ = 0;
		copyConstructions_ = 0;
		moveConstructions_ = 0;
		assignments_ = 0;
		moveAssignments_ = 0;
	}

  private:
	static int counter_;

	static int constructions_;
	static int destructions_;
	static int copyConstructions_;
	static int moveConstructions_;
	static int assignments_;
	static int moveAssignments_;

	bool initialized_ = false;
};

class RefCountedClassHashFunc
{
  public:
	int operator()(const RefCounted &key) const { return RefCounted::counter(); }
};

void printRefCounters()
{
#if PRINT_ALL_COUNTERS
	const int totalConstructions = RefCounted::constructions() + RefCounted::copyConstructions() +
	                               RefCounted::moveConstructions() + RefCounted::assignments() + RefCounted::moveAssignments();

	printf("RefCounter: %d (C %d\\D %d)\n", RefCounted::counter(), totalConstructions, RefCounted::destructions());
	printf("Construction\\Copy\\Move: %d\\%d\\%d\n", RefCounted::constructions(),
	       RefCounted::copyConstructions(), RefCounted::moveConstructions());
	printf("Assign\\MoveAssign: %d\\%d\n", RefCounted::assignments(), RefCounted::moveAssignments());
#else
	printf("RefCounter: %d\n", RefCounted::counter());
#endif
	RefCounted::reset();
}

}

#endif
