#include "gtest_atomic.h"
#include "test_thread_functions.h"

namespace {

using AtomicTypes = ::testing::Types<nctl::Atomic32, nctl::AtomicU32, nctl::Atomic64, nctl::AtomicU64>;
TYPED_TEST_SUITE(AtomicTest, AtomicTypes);

template <typename TypeParam>
class AtomicTest : public ::testing::Test
{
  public:
	AtomicTest()
	    : nonAtom_(0), tr_(this) {}

	TypeParam atom_;
	TypeParam lock_;
	typename TypeParam::Type nonAtom_;
	TypeParam array_[ArrayLength];
	ThreadRunner<NumThreads> tr_;
};

TYPED_TEST(AtomicTest, Increment)
{
	using TestClass = AtomicTest<TypeParam>;
	using IntType = typename TypeParam::Type;

	this->tr_.runThreads([](void *arg) -> ThreadRunner<NumThreads>::threadFuncRet {
		TestClass *obj = static_cast<TestClass *>(arg);
		for (unsigned int i = 0; i < NumIterations; i++)
		{
			obj->atom_++;
			obj->nonAtom_++;
		}
		return obj->tr_.retFunc();
	});

	const IntType value = this->atom_;
	printf("Incrementing the atomic with %u threads and %u iterations: ", NumThreads, NumIterations);
	printValue(value);
	ASSERT_EQ(value, NumThreads * NumIterations);
#if 0
	printf("Incrementing the non-atomic with %u threads and %u iterations: %d\n", NumThreads, NumIterations, nonAtom_);
	EXPECT_NE(nonAtom_, NumThreads * NumIterations);
#endif
}

TYPED_TEST(AtomicTest, Decrement)
{
	using TestClass = AtomicTest<TypeParam>;
	using IntType = typename TypeParam::Type;

	this->atom_.store(NumThreads * NumIterations);

	this->tr_.runThreads([](void *arg) -> ThreadRunner<NumThreads>::threadFuncRet {
		TestClass *obj = static_cast<TestClass *>(arg);
		for (unsigned int i = 0; i < NumIterations; i++)
		{
			obj->atom_--;
			obj->nonAtom_--;
		}
		return obj->tr_.retFunc();
	});

	const IntType value = this->atom_;
	printf("Decrementing the atomic with %u threads and %u iterations: ", NumThreads, NumIterations);
	printValue(value);
	ASSERT_EQ(value, 0);
#if 0
	printf("Decrementing the non-atomic with %u threads and %u iterations: %d\n", NumThreads, NumIterations, nonAtom_);
	EXPECT_NE(nonAtom_, 0);
#endif
}

TYPED_TEST(AtomicTest, BitfieldOr)
{
	using TestClass = AtomicTest<TypeParam>;
	using IntType = typename TypeParam::Type;

	const unsigned int TotalBits = ArrayLength * sizeof(IntType) * 8;
	ThreadRunner<TotalBits> tr(this);

	printf("Clearing all bits in the array\n");
	for (unsigned int i = 0; i < ArrayLength; i++)
		this->array_[i].store(0);

	tr.runThreadsWithIndex([](void *arg) -> typename ThreadRunner<TotalBits>::threadFuncRet {
		const unsigned int BucketBits = sizeof(nctl::Atomic32::Type) * 8;
		typename ThreadRunner<TotalBits>::ThreadIndexAndPointer *data = static_cast<typename ThreadRunner<TotalBits>::ThreadIndexAndPointer *>(arg);
		const unsigned int threadIndex = data->threadIndex;
		TestClass *obj = static_cast<TestClass *>(data->argument);

		const unsigned int index = threadIndex / BucketBits;
		const IntType mask = 1 << (threadIndex % BucketBits);
		obj->array_[index].fetchOr(mask);

		delete data; // Each thread needs to delete the structure allocated by `runThreadsWithIndex`
		return obj->tr_.retFunc(); // using `tr_` return function instead of test's `tr` one
	});

	printf("Veryfying that all bits in the array have been set\n");
	for (unsigned int i = 0; i < ArrayLength; i++)
	{
		printf("Array element %u:", i);
		printHexValue(this->array_[i].load());
		EXPECT_EQ(this->array_[i].load(), TypeParam(~0));
	}
}

TYPED_TEST(AtomicTest, BitfieldAnd)
{
	using TestClass = AtomicTest<TypeParam>;
	using IntType = typename TypeParam::Type;

	const unsigned int TotalBits = ArrayLength * sizeof(nctl::Atomic32::Type) * 8;
	ThreadRunner<TotalBits> tr(this);

	printf("Setting all bits in the array\n");
	for (unsigned int i = 0; i < ArrayLength; i++)
		this->array_[i].store(TypeParam(~0));

	tr.runThreadsWithIndex([](void *arg) -> ThreadRunner<TotalBits>::threadFuncRet {
		const unsigned int BucketBits = sizeof(nctl::Atomic32::Type) * 8;
		ThreadRunner<TotalBits>::ThreadIndexAndPointer *data = static_cast<ThreadRunner<TotalBits>::ThreadIndexAndPointer *>(arg);
		const unsigned int threadIndex = data->threadIndex;
		TestClass *obj = static_cast<TestClass *>(data->argument);

		const unsigned int index = threadIndex / BucketBits;
		const IntType mask = ~(1 << (threadIndex % BucketBits));
		obj->array_[index].fetchAnd(mask);

		delete data; // Each thread needs to delete the structure allocated by `runThreadsWithIndex`
		return obj->tr_.retFunc(); // using `tr_` return function instead of test's `tr` one
	});

	printf("Veryfying that all bits in the array have been cleared\n");
	for (unsigned int i = 0; i < ArrayLength; i++)
	{
		printf("Array element %u:", i);
		printHexValue(this->array_[i].load());
		EXPECT_EQ(this->array_[i].load(), 0);
	}
}

TYPED_TEST(AtomicTest, BitfieldXor)
{
	using TestClass = AtomicTest<TypeParam>;
	using IntType = typename TypeParam::Type;

	const unsigned int TotalBits = ArrayLength * sizeof(nctl::Atomic32::Type) * 8;
	ThreadRunner<TotalBits> tr(this);

	printf("Clearing all bits in the array\n");
	for (unsigned int i = 0; i < ArrayLength; i++)
		this->array_[i].store(0);

	tr.runThreadsWithIndex([](void *arg) -> ThreadRunner<TotalBits>::threadFuncRet {
		const unsigned int BucketBits = sizeof(nctl::Atomic32::Type) * 8;
		ThreadRunner<TotalBits>::ThreadIndexAndPointer *data = static_cast<ThreadRunner<TotalBits>::ThreadIndexAndPointer *>(arg);
		const unsigned int threadIndex = data->threadIndex;
		TestClass *obj = static_cast<TestClass *>(data->argument);

		const unsigned int index = threadIndex / BucketBits;
		const IntType mask = 1 << (threadIndex % BucketBits);
		obj->array_[index].fetchXor(mask);

		delete data; // Each thread needs to delete the structure allocated by `runThreadsWithIndex`
		return obj->tr_.retFunc(); // using `tr_` return function instead of test's `tr` one
	});

	printf("Veryfying that all bits in the array have been set\n");
	for (unsigned int i = 0; i < ArrayLength; i++)
	{
		printf("Array element %u:", i);
		printHexValue(this->array_[i].load());
		EXPECT_EQ(this->array_[i].load(), TypeParam(~0));
	}
}

TYPED_TEST(AtomicTest, CompareAndExchange)
{
	using TestClass = AtomicTest<TypeParam>;
	using IntType = typename TypeParam::Type;

	this->tr_.runThreads([](void *arg) -> ThreadRunner<NumThreads>::threadFuncRet {
		TestClass *obj = static_cast<TestClass *>(arg);
		for (unsigned int i = 0; i < NumIterations; i++)
		{
			IntType cmpValue;
			do {
				cmpValue = 0;
			} while (!obj->lock_.cmpExchange(cmpValue, 1));

			IntType value = obj->atom_;
			value++;
			obj->atom_ = value;
			obj->nonAtom_++;

			obj->lock_--;
		}
		return obj->tr_.retFunc();
	});

	const IntType value = this->atom_;
	printf("Incrementing the atomic with %u threads and %u iterations and a compare-exchange lock: ", NumThreads, NumIterations);
	printValue(value);
	ASSERT_EQ(value, NumThreads * NumIterations);
	printf("Incrementing the non-atomic with %u threads and %u iterations and a compare-exchange lock: ", NumThreads, NumIterations);
	printValue(this->nonAtom_);
	ASSERT_EQ(this->nonAtom_, NumThreads * NumIterations);
}

}
