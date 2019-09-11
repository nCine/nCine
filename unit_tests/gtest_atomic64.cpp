#include "gtest_atomic.h"
#include "test_thread_functions.h"

namespace {

class AtomicTest64 : public ::testing::Test
{
  public:
	AtomicTest64()
	    : nonAtom_(0), tr_(this) {}

	nctl::Atomic64 atom_;
	nctl::Atomic64 lock_;
	int64_t nonAtom_;
	ThreadRunner<NumThreads> tr_;
};

TEST_F(AtomicTest64, Increment)
{
	tr_.runThreads([](void *arg) -> ThreadRunner<NumThreads>::threadFuncRet {
		AtomicTest64 *obj = static_cast<AtomicTest64 *>(arg);
		for (unsigned int i = 0; i < NumIterations; i++)
		{
			obj->atom_.operator++(0);
			obj->nonAtom_++;
		}
		return static_cast<AtomicTest64 *>(arg)->tr_.retFunc();
	});

	const int64_t value = atom_;
	printf("Incrementing the atomic with %u threads and %u iterations: %ld\n", NumThreads, NumIterations, value);
	ASSERT_EQ(value, NumThreads * NumIterations);
#if 0
	printf("Incrementing the non-atomic with %u threads and %u iterations: %ld\n", NumThreads, NumIterations, nonAtom_);
	EXPECT_NE(nonAtom_, NumThreads * NumIterations);
#endif
}

TEST_F(AtomicTest64, Decrement)
{
	tr_.runThreads([](void *arg) -> ThreadRunner<NumThreads>::threadFuncRet {
		AtomicTest64 *obj = static_cast<AtomicTest64 *>(arg);
		for (unsigned int i = 0; i < NumIterations; i++)
		{
			obj->atom_.operator--(0);
			obj->nonAtom_--;
		}
		return static_cast<AtomicTest64 *>(arg)->tr_.retFunc();
	});

	const int64_t value = atom_;
	printf("Decrementing the atomic with %u threads and %u iterations: %ld\n", NumThreads, NumIterations, value);
	ASSERT_EQ(value, -(NumThreads * NumIterations));
#if 0
	printf("Decrementing the non-atomic with %u threads and %u iterations: %ld\n", NumThreads, NumIterations, nonAtom_);
	EXPECT_NE(nonAtom_, -(NumThreads * NumIterations));
#endif
}

TEST_F(AtomicTest64, CompareAndExchange)
{
	tr_.runThreads([](void *arg) -> ThreadRunner<NumThreads>::threadFuncRet {
		AtomicTest64 *obj = static_cast<AtomicTest64 *>(arg);
		for (unsigned int i = 0; i < NumIterations; i++)
		{
			while (!obj->lock_.cmpExchange(1, 0)) {}

			int64_t value = obj->atom_;
			value++;
			obj->atom_ = value;
			obj->nonAtom_++;

			obj->lock_--;
		}
		return static_cast<AtomicTest64 *>(arg)->tr_.retFunc();
	});

	const int64_t value = atom_;
	printf("Incrementing the atomic with %u threads and %u iterations and a compare-exchange lock: %ld\n", NumThreads, NumIterations, value);
	ASSERT_EQ(value, NumThreads * NumIterations);
	printf("Incrementing the non-atomic with %u threads and %u iterations and a compare-exchange lock: %ld\n", NumThreads, NumIterations, nonAtom_);
	ASSERT_EQ(nonAtom_, NumThreads * NumIterations);
}

}
