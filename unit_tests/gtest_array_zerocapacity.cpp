#include "gtest_array.h"

namespace {

class ArrayZeroCapacityTest : public ::testing::Test
{
  public:
	ArrayZeroCapacityTest()
	    : array_(0) {}

	nctl::Array<int> array_;
};

#ifndef __EMSCRIPTEN__
	#ifdef NCINE_DEBUG
TEST(ArrayZeroCapacityDeathTest, SubscriptAccessBeyondSize)
{
	printf("Trying to access an element of an array with zero capacity\n");
	nctl::Array<int> array(0);

	ASSERT_DEATH(array[1] = 1, "");
}

TEST(ArrayZeroCapacityDeathTest, SubscriptAccessConstBeyondSize)
{
	printf("Trying to access an element of an array with zero capacity\n");
	nctl::Array<int> array(0);

	const nctl::Array<int> &constArray = array;

	ASSERT_DEATH(constArray[1], "");
}
	#endif

TEST(ArrayZeroCapacityDeathTest, AccessBeyondSize)
{
	printf("Trying to access an element of an array with zero capacity\n");
	nctl::Array<int> array(0);

	ASSERT_DEATH(array.at(1) = 1, "");
}

TEST(ArrayZeroCapacityDeathTest, AccessConstBeyondSize)
{
	printf("Trying to access an element of an array with zero capacity\n");
	nctl::Array<int> array(0);

	const nctl::Array<int> &constArray = array;

	ASSERT_DEATH(constArray.at(1), "");
}

TEST(ArrayZeroCapacityDeathTest, AccessConstAtSize)
{
	printf("Trying to access an element of a const array at size\n");
	nctl::Array<int> array(Capacity);

	const nctl::Array<int> &constArray = array;

	ASSERT_DEATH(constArray.at(0), "");
}
#endif

TEST_F(ArrayZeroCapacityTest, AccessEmptyWithinSize)
{
	printf("Accessing at array size to add elements\n");
	array_.at(0) = 0;
	printArray(array_);

	ASSERT_EQ(array_.capacity(), 1);
	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(array_.at(0), 0);
}

TEST_F(ArrayZeroCapacityTest, SubscriptAccessEmptyWithinSize)
{
	printf("Accessing at array size to add elements\n");
	array_[0] = 0;
	printArray(array_);

	ASSERT_EQ(array_.capacity(), 1);
	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(array_[0], 0);
}

TEST_F(ArrayZeroCapacityTest, SubscriptAccessNotGrowing)
{
	printf("Trying to access an element beyond the capacity of zero capacity array\n");
	nctl::Array<int> array(0, nctl::ArrayMode::FIXED_CAPACITY);
	array[0] = 0;

	ASSERT_EQ(array.capacity(), 1);
	ASSERT_EQ(array.size(), 1);
	ASSERT_EQ(array[0], 0);
}

TEST_F(ArrayZeroCapacityTest, SetSizeOnFixed)
{
	printf("Setting the size of a fixed capacity array with zero capacity\n");
	nctl::Array<int> array(0, nctl::ArrayMode::FIXED_CAPACITY);
	array_.setSize(1);
	array[0] = 0;

	printf("Trying to extend the size of a fixed capacity array\n");
	array_.setSize(Capacity);

	ASSERT_EQ(array.capacity(), 1);
	ASSERT_EQ(array.size(), 1);
}

TEST_F(ArrayZeroCapacityTest, SetCapacityOnFixed)
{
	printf("Setting a new capacity for a fixed capacity array with zero capacity\n");
	nctl::Array<int> array(0, nctl::ArrayMode::FIXED_CAPACITY);
	array.setCapacity(Capacity);

	printf("Setting a new capacity for a fixed capacity array\n");
	array.setCapacity(Capacity * 2);

	ASSERT_EQ(array.capacity(), Capacity);
}

TEST_F(ArrayZeroCapacityTest, SetSameCapacity)
{
	printf("Setting a new capacity equal to the original one for a zero capacity array\n");
	array_.setCapacity(0);
	printArray(array_);

	ASSERT_EQ(array_.capacity(), 0);
	ASSERT_EQ(array_.size(), 0);
}

TEST_F(ArrayZeroCapacityTest, PushBack)
{
	printf("Inserting at the back\n");
	array_.pushBack(1);
	printArray(array_);

	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(array_.capacity(), 1);
}

TEST_F(ArrayZeroCapacityTest, InsertFirstAndLast)
{
	printf("Inserting as first and last\n");
	array_.insertAt(0, 0);
	array_.insertAt(array_.size(), 1);
	printArray(array_);

	ASSERT_EQ(array_[0], 0);
	ASSERT_EQ(array_[1], 1);
	ASSERT_EQ(array_.size(), 2);
	ASSERT_EQ(array_.capacity(), 2);
}

TEST_F(ArrayZeroCapacityTest, ExtendAndShrinkSize)
{
	printf("Extending the size by two elements\n");
	const unsigned int oldSize = array_.size();
	array_.setSize(oldSize + 2);
	// No "hole" when setting this element
	array_[oldSize + 1] = 1;
	array_[oldSize] = 0;
	printArray(array_);
	ASSERT_EQ(array_.size(), oldSize + 2);

	printf("Restoring previous size\n");
	array_.setSize(oldSize);
	printArray(array_);
	ASSERT_EQ(array_.size(), oldSize);
}

}
