#include "gtest_sparseset.h"

namespace {

class SparseSetTest : public ::testing::Test
{
  public:
	SparseSetTest()
	    : sparseset_(Capacity, MaxValue) {}

  protected:
	void SetUp() override { initSparseSet(sparseset_); }

	SparseSetTestType sparseset_;
};

#ifndef __EMSCRIPTEN__
TEST(SparseSetDeathTest, ZeroCapacity)
{
	printf("Creating a sparseset of zero capacity\n");
	ASSERT_DEATH(SparseSetTestType newSparseset(0, MaxValue), "");
}

TEST(SparseSetDeathTest, MaximumValueSmallerThanCapacity)
{
	printf("Creating a sparseset with a maximum value smaller than capacity\n");
	ASSERT_DEATH(SparseSetTestType newSparseset(Capacity, Capacity / 2), "");
}
#endif

TEST_F(SparseSetTest, MaxValue)
{
	const unsigned int maxValue = sparseset_.maxValue();
	printf("Maximum Value: %u\n", maxValue);

	ASSERT_EQ(maxValue, MaxValue);
}

TEST_F(SparseSetTest, Capacity)
{
	const unsigned int capacity = sparseset_.capacity();
	printf("Capacity: %u\n", capacity);

	ASSERT_EQ(capacity, Capacity);
}

TEST_F(SparseSetTest, Size)
{
	const unsigned int size = sparseset_.size();
	printf("Size: %u\n", size);

	ASSERT_EQ(size, Size);
	ASSERT_EQ(calcSize(sparseset_), Size);
}

TEST_F(SparseSetTest, LoadFactor)
{
	const float loadFactor = sparseset_.loadFactor();
	printf("Size: %u, Capacity: %u, Load Factor: %f\n", Size, Capacity, loadFactor);

	ASSERT_FLOAT_EQ(loadFactor, Size / static_cast<float>(Capacity));
}

TEST_F(SparseSetTest, Clear)
{
	ASSERT_FALSE(sparseset_.isEmpty());
	sparseset_.clear();
	printSparseSet(sparseset_);
	ASSERT_TRUE(sparseset_.isEmpty());
	ASSERT_EQ(sparseset_.size(), 0u);
	ASSERT_EQ(sparseset_.capacity(), Capacity);
}

TEST_F(SparseSetTest, InsertElements)
{
	printf("Inserting elements\n");
	for (unsigned int i = Size; i < Size * 2; i++)
		sparseset_.insert(i);

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_TRUE(sparseset_.contains(i));

	ASSERT_EQ(sparseset_.size(), Size * 2);
	ASSERT_EQ(calcSize(sparseset_), Size * 2);
}

TEST_F(SparseSetTest, FailInsertElements)
{
	printf("Trying to insert elements already in the sparseset\n");
	for (unsigned int i = 0; i < Size * 2; i++)
		sparseset_.insert(i);

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_TRUE(sparseset_.contains(i));

	ASSERT_EQ(sparseset_.size(), Size * 2);
	ASSERT_EQ(calcSize(sparseset_), Size * 2);
}

TEST_F(SparseSetTest, RemoveElements)
{
	printf("Original size: %u\n", sparseset_.size());
	printf("Removing a couple elements\n");
	printf("New size: %u\n", sparseset_.size());
	sparseset_.remove(5);
	sparseset_.remove(7);
	printSparseSet(sparseset_);

	ASSERT_FALSE(sparseset_.contains(5));
	ASSERT_FALSE(sparseset_.contains(7));
	ASSERT_EQ(sparseset_.size(), Size - 2);
	ASSERT_EQ(calcSize(sparseset_), Size - 2);
}

TEST_F(SparseSetTest, RehashExtend)
{
	const float loadFactor = sparseset_.loadFactor();
	printf("Original size: %u, capacity: %u, load factor: %f\n", sparseset_.size(), sparseset_.capacity(), sparseset_.loadFactor());
	printSparseSet(sparseset_);
	ASSERT_EQ(sparseset_.capacity(), Capacity);

	printf("Doubling capacity by rehashing\n");
	sparseset_.rehash(sparseset_.capacity() * 2);
	printf("New size: %u, capacity: %u, load factor: %f\n", sparseset_.size(), sparseset_.capacity(), sparseset_.loadFactor());
	printSparseSet(sparseset_);

	ASSERT_EQ(sparseset_.capacity(), Capacity * 2);
	ASSERT_EQ(sparseset_.size(), Size);
	ASSERT_EQ(calcSize(sparseset_), Size);
	ASSERT_FLOAT_EQ(sparseset_.loadFactor(), loadFactor * 0.5f);

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_TRUE(sparseset_.contains(i));
}

TEST_F(SparseSetTest, RehashShrink)
{
	printf("Original size: %u, capacity: %u, load factor: %f\n", sparseset_.size(), sparseset_.capacity(), sparseset_.loadFactor());
	printSparseSet(sparseset_);
	ASSERT_EQ(sparseset_.capacity(), Capacity);

	printf("Set capacity to current size by rehashing\n");
	sparseset_.rehash(sparseset_.size());
	printf("New size: %u, capacity: %u, load factor: %f\n", sparseset_.size(), sparseset_.capacity(), sparseset_.loadFactor());
	printSparseSet(sparseset_);

	ASSERT_EQ(sparseset_.capacity(), Size);
	ASSERT_EQ(sparseset_.size(), Size);
	ASSERT_EQ(calcSize(sparseset_), Size);
	ASSERT_FLOAT_EQ(sparseset_.loadFactor(), 1.0f);

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_TRUE(sparseset_.contains(i));
}

TEST_F(SparseSetTest, CopyConstruction)
{
	printf("Creating a new sparseset with copy construction\n");
	SparseSetTestType newSparseset(sparseset_);
	printSparseSet(newSparseset);

	assertSparseSetsAreEqual(sparseset_, newSparseset);
	ASSERT_EQ(sparseset_.size(), Size);
	ASSERT_EQ(calcSize(sparseset_), Size);
	ASSERT_EQ(newSparseset.size(), Size);
	ASSERT_EQ(calcSize(newSparseset), Size);
}

TEST_F(SparseSetTest, MoveConstruction)
{
	printf("Creating a new sparseset with move construction\n");
	SparseSetTestType newSparseset = nctl::move(sparseset_);
	printSparseSet(newSparseset);

	ASSERT_EQ(sparseset_.size(), 0);
	ASSERT_EQ(newSparseset.capacity(), Capacity);
	ASSERT_EQ(newSparseset.size(), Size);
	ASSERT_EQ(calcSize(newSparseset), Size);
}

TEST_F(SparseSetTest, AssignmentOperator)
{
	printf("Creating a new sparseset with the assignment operator\n");
	SparseSetTestType newSparseset(Capacity, MaxValue);
	newSparseset = sparseset_;
	printSparseSet(newSparseset);

	assertSparseSetsAreEqual(sparseset_, newSparseset);
	ASSERT_EQ(sparseset_.size(), Size);
	ASSERT_EQ(calcSize(sparseset_), Size);
	ASSERT_EQ(newSparseset.size(), Size);
	ASSERT_EQ(calcSize(newSparseset), Size);
}

TEST_F(SparseSetTest, MoveAssignmentOperator)
{
	printf("Creating a new sparseset with the move assignment operator\n");
	SparseSetTestType newSparseset(Capacity, MaxValue);
	newSparseset = nctl::move(sparseset_);
	printSparseSet(newSparseset);

	ASSERT_EQ(sparseset_.size(), 0);
	ASSERT_EQ(newSparseset.capacity(), Capacity);
	ASSERT_EQ(newSparseset.size(), Size);
	ASSERT_EQ(calcSize(newSparseset), Size);
}

TEST_F(SparseSetTest, SelfAssignment)
{
	printf("Assigning the sparseset to itself with the assignment operator\n");
	sparseset_ = sparseset_;
	printSparseSet(sparseset_);

	ASSERT_EQ(sparseset_.size(), Size);
	ASSERT_EQ(calcSize(sparseset_), Size);
	ASSERT_EQ(sparseset_.capacity(), Capacity);
}

TEST_F(SparseSetTest, Contains)
{
	const int key = 1;
	const bool found = sparseset_.contains(key);
	printf("Key %d is in the sparseset: %d\n", key, found);

	ASSERT_TRUE(found);
}

TEST_F(SparseSetTest, DoesNotContain)
{
	const int key = 10;
	const bool found = sparseset_.contains(key);
	printf("Key %d is in the sparseset: %d\n", key, found);

	ASSERT_FALSE(found);
}

TEST_F(SparseSetTest, Find)
{
	const int key = 1;
	const int value = sparseset_.find(key);
	printf("Key %d is in the sparseset: %d - Value: %d\n", key, value != SparseSetTestType::NotFound, value);

	ASSERT_TRUE(value != SparseSetTestType::NotFound);
	ASSERT_EQ(value, key);
}

TEST_F(SparseSetTest, CannotFind)
{
	const int key = 10;
	const int value = sparseset_.find(key);
	printf("Key %d is in the sparseset: %d\n", key, value != SparseSetTestType::NotFound);

	ASSERT_FALSE(value != SparseSetTestType::NotFound);
}

TEST_F(SparseSetTest, FillCapacity)
{
	printf("Creating a new sparseset to fill up to capacity (%u elements)\n", Capacity);
	SparseSetTestType newSparseset(Capacity, MaxValue);

	for (unsigned int i = 0; i < Capacity; i++)
		newSparseset.insert(i);

	ASSERT_EQ(newSparseset.size(), Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_TRUE(newSparseset.contains(i));
}

TEST_F(SparseSetTest, RemoveAllFromFull)
{
	printf("Creating a new sparseset to fill up to capacity (%u elements)\n", Capacity);
	SparseSetTestType newSparseset(Capacity, MaxValue);

	for (unsigned int i = 0; i < Capacity; i++)
		newSparseset.insert(i);

	printf("Removing all elements from the sparseset\n");
	for (unsigned int i = 0; i < Capacity; i++)
		newSparseset.remove(i);

	ASSERT_EQ(newSparseset.size(), 0);
	ASSERT_EQ(calcSize(newSparseset), 0);
}

const int BigCapacity = 512;
const int BigMaxValue = 1024;
const int LastElement = BigCapacity / 2;

TEST_F(SparseSetTest, StressRemove)
{
	printf("Creating a new sparseset with a capacity of %u and filled up to %u elements\n", BigCapacity, LastElement);
	SparseSetTestType newSparseset(BigCapacity, BigMaxValue);

	for (int i = 0; i < LastElement; i++)
		newSparseset.insert(i);
	ASSERT_EQ(newSparseset.size(), LastElement);

	printf("Removing all elements from the sparseset\n");
	for (int i = 0; i < LastElement; i++)
	{
		newSparseset.remove(i);
		ASSERT_EQ(newSparseset.size(), LastElement - i - 1);

		for (int j = i + 1; j < LastElement; j++)
			ASSERT_TRUE(newSparseset.contains(j));
		for (int j = 0; j < i + 1; j++)
			ASSERT_FALSE(newSparseset.contains(j));
	}

	ASSERT_EQ(newSparseset.size(), 0);
}

TEST_F(SparseSetTest, StressReverseRemove)
{
	printf("Creating a new sparseset with a capacity of %u and filled up to %u elements\n", BigCapacity, LastElement);
	SparseSetTestType newSparseset(BigCapacity, BigMaxValue);

	for (int i = 0; i < LastElement; i++)
		newSparseset.insert(i);
	ASSERT_EQ(newSparseset.size(), LastElement);

	printf("Removing all elements from the sparseset\n");
	for (int i = LastElement - 1; i >= 0; i--)
	{
		newSparseset.remove(i);
		ASSERT_EQ(newSparseset.size(), i);

		for (int j = i - 1; j >= 0; j--)
			ASSERT_TRUE(newSparseset.contains(j));
		for (int j = LastElement; j >= i; j--)
			ASSERT_FALSE(newSparseset.contains(j));
	}

	ASSERT_EQ(newSparseset.size(), 0);
}

}
