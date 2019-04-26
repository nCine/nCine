#include "gtest_atomic.h"
#include "test_thread_functions.h"

namespace nc = ncine;

namespace {

class AtomicTest32 : public ::testing::Test
{
  public:
	AtomicTest32()
	    : nonAtom_(0), tr_(this) {}

	nctl::Atomic32 atom_;
	nctl::Atomic32 lock_;
	int32_t nonAtom_;
	ThreadRunner<NumThreads> tr_;
};

TEST_F(AtomicTest32, Increment)
{
	tr_.runThreads([](void *arg) -> ThreadRunner<NumThreads>::threadFuncRet {
		AtomicTest32 *obj = static_cast<AtomicTest32 *>(arg);
		for (unsigned int i = 0; i < NumIterations; i++)
		{
			obj->atom_.operator++(0);
			obj->nonAtom_++;
		}
		return static_cast<AtomicTest32 *>(arg)->tr_.retFunc();
	});

	const int32_t value = atom_;
	printf("Incrementing the atomic with %u threads and %u iterations: %d\n", NumThreads, NumIterations, value);
	ASSERT_EQ(value, NumThreads * NumIterations);
	printf("Incrementing the non-atomic with %u threads and %u iterations: %d\n", NumThreads, NumIterations, nonAtom_);
	EXPECT_NE(nonAtom_, NumThreads * NumIterations);
}

TEST_F(AtomicTest32, Decrement)
{
	tr_.runThreads([](void *arg) -> ThreadRunner<NumThreads>::threadFuncRet {
		AtomicTest32 *obj = static_cast<AtomicTest32 *>(arg);
		for (unsigned int i = 0; i < NumIterations; i++)
		{
			obj->atom_.operator--(0);
			obj->nonAtom_--;
		}
		return static_cast<AtomicTest32 *>(arg)->tr_.retFunc();
	});

	const int32_t value = atom_;
	printf("Decrementing the atomic with %u threads and %u iterations: %d\n", NumThreads, NumIterations, value);
	ASSERT_EQ(value, -(NumThreads * NumIterations));
	printf("Decrementing the non-atomic with %u threads and %u iterations: %d\n", NumThreads, NumIterations, nonAtom_);
	EXPECT_NE(nonAtom_, -(NumThreads * NumIterations));
}

TEST_F(AtomicTest32, CompareAndExchange)
{
	tr_.runThreads([](void *arg) -> ThreadRunner<NumThreads>::threadFuncRet {
		AtomicTest32 *obj = static_cast<AtomicTest32 *>(arg);
		for (unsigned int i = 0; i < NumIterations; i++)
		{
			while (!obj->lock_.cmpExchange(1, 0)) {}

			int32_t value = obj->atom_;
			value++;
			obj->atom_ = value;
			obj->nonAtom_++;

			obj->lock_--;
		}
		return static_cast<AtomicTest32 *>(arg)->tr_.retFunc();
	});

	const int32_t value = atom_;
	printf("Incrementing the atomic with %u threads and %u iterations and a compare-exchange lock: %d\n", NumThreads, NumIterations, value);
	ASSERT_EQ(value, NumThreads * NumIterations);
	printf("Incrementing the non-atomic with %u threads and %u iterations and a compare-exchange lock: %d\n", NumThreads, NumIterations, nonAtom_);
	ASSERT_EQ(nonAtom_, NumThreads * NumIterations);
}

}
