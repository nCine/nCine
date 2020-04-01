#include "gtest_array.h"

namespace {

class ArrayTest : public ::testing::Test
{
  public:
	ArrayTest()
	    : array_(Capacity) {}

  protected:
	void SetUp() override { initArray(array_); }

	nctl::Array<int> array_;
};

#ifndef __EMSCRIPTEN__
	#ifdef NCINE_DEBUG
TEST(ArrayDeathTest, SubscriptAccessBeyondSize)
{
	printf("Trying to access an element within capacity but beyond size\n");
	nctl::Array<int> array(Capacity);
	array[0] = 0;

	ASSERT_DEATH(array[5] = 1, "");
}

TEST(ArrayDeathTest, SubscriptAccessConstBeyondSize)
{
	printf("Trying to access an element of a const array within capacity but beyond size\n");
	nctl::Array<int> array(Capacity);
	array[0] = 0;

	const nctl::Array<int> &constArray = array;

	ASSERT_DEATH(constArray[5], "");
}

TEST(ArrayDeathTest, SubscriptAccessConstAtSize)
{
	printf("Trying to access an element of a const array within capacity but at size\n");
	nctl::Array<int> array(Capacity);
	array[0] = 0;

	const nctl::Array<int> &constArray = array;

	ASSERT_DEATH(constArray[1], "");
}
	#endif

TEST(ArrayDeathTest, AccessBeyondSize)
{
	printf("Trying to access an element within capacity but beyond size\n");
	nctl::Array<int> array(Capacity);
	array[0] = 0;

	ASSERT_DEATH(array.at(5) = 1, "");
}

TEST(ArrayDeathTest, AccessConstBeyondSize)
{
	printf("Trying to access an element of a const array within capacity but beyond size\n");
	nctl::Array<int> array(Capacity);
	array[0] = 0;

	const nctl::Array<int> &constArray = array;

	ASSERT_DEATH(constArray.at(5), "");
}

TEST(ArrayDeathTest, AccessConstAtSize)
{
	printf("Trying to access an element of a const array at size\n");
	nctl::Array<int> array(Capacity);
	array[0] = 0;

	const nctl::Array<int> &constArray = array;

	ASSERT_DEATH(constArray.at(1), "");
}
#endif

TEST_F(ArrayTest, AccessEmptyWithinSize)
{
	printf("Accessing at array size to add elements\n");
	nctl::Array<int> newArray(Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		newArray.at(i) = i;
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(newArray.at(i), static_cast<int>(i));
}

TEST_F(ArrayTest, AccessWithinSize)
{
	printf("Accessing an element within the array size\n");

	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(array_.at(i), static_cast<int>(i));
}

TEST_F(ArrayTest, AccessWithPointer)
{
	printf("Accessing an array element with a pointer\n");

	int *ptr = array_.data();
	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(ptr[i], static_cast<int>(i));
}

TEST_F(ArrayTest, SubscriptAccessEmptyWithinSize)
{
	printf("Accessing at array size to add elements\n");
	nctl::Array<int> newArray(Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		newArray[i] = static_cast<int>(i);
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(newArray.at(i), static_cast<int>(i));
}

TEST_F(ArrayTest, SubscriptAccessNotGrowing)
{
	printf("Trying to access an element beyond the capacity of a fixed capacity array\n");
	nctl::Array<int> array(1, nctl::ArrayMode::FIXED_CAPACITY);
	array[0] = 0;
	array[1] = 1;

	ASSERT_EQ(array.capacity(), 1);
	ASSERT_EQ(array.size(), 1);
}

TEST_F(ArrayTest, SetSizeOnFixed)
{
	printf("Trying to extend the size of a fixed capacity array\n");
	nctl::Array<int> array(1, nctl::ArrayMode::FIXED_CAPACITY);
	array[0] = 0;
	array_.setSize(Capacity);

	ASSERT_EQ(array.capacity(), 1);
	ASSERT_EQ(array.size(), 1);
}

TEST_F(ArrayTest, SetCapacityOnFixed)
{
	printf("Setting a new capacity for a fixed capacity array\n");
	nctl::Array<int> array(Capacity, nctl::ArrayMode::FIXED_CAPACITY);
	array.setCapacity(Capacity * 2);

	ASSERT_EQ(array.capacity(), Capacity);
}

TEST_F(ArrayTest, SetSameCapacity)
{
	printf("Setting a new capacity equal to the original one\n");
	array_.setCapacity(Capacity);
	printArray(array_);

	ASSERT_TRUE(isUnmodified(array_));
	ASSERT_EQ(array_.capacity(), Capacity);
	ASSERT_EQ(array_.size(), Capacity);
}

TEST_F(ArrayTest, SetHalfCapacity)
{
	printf("Shrinking the array by setting half the original capacity\n");
	array_.setCapacity(Capacity / 2);
	printArray(array_);

	ASSERT_EQ(array_.capacity(), Capacity / 2);
	ASSERT_EQ(array_.size(), Capacity / 2);
	for (unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i);
}

TEST_F(ArrayTest, SetZeroCapacity)
{
	printf("Freeing all memory of the array by setting the capacity to zero\n");
	array_.setCapacity(0);
	printArray(array_);

	ASSERT_EQ(array_.capacity(), 0);
	ASSERT_EQ(array_.size(), 0);
}

TEST_F(ArrayTest, WritingBeyondCapacity)
{
	printf("Writing beyond capacity\n");
	array_[Capacity] = static_cast<int>(Capacity);
	printArray(array_);

	ASSERT_TRUE(isUnmodified(array_));
	ASSERT_EQ(array_[Capacity], static_cast<int>(Capacity));
	ASSERT_EQ(array_.size(), Capacity + 1);
	ASSERT_EQ(array_.capacity(), Capacity * 2);
}

TEST_F(ArrayTest, FrontElement)
{
	const int front = array_.front();
	printf("Retrieving the front element: %d\n", front);

	ASSERT_TRUE(isUnmodified(array_));
	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_EQ(array_.capacity(), Capacity);
	ASSERT_EQ(front, array_[0]);
}

TEST_F(ArrayTest, BackElement)
{
	const int back = array_.back();
	printf("Retrieving the back element: %d\n", back);

	ASSERT_TRUE(isUnmodified(array_));
	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_EQ(array_.capacity(), Capacity);
	ASSERT_EQ(back, array_[array_.size() - 1]);
}

TEST_F(ArrayTest, PushBack)
{
	printf("Inserting at the back\n");
	array_.pushBack(Capacity + 1);
	printArray(array_);

	ASSERT_TRUE(isUnmodified(array_));
	ASSERT_EQ(array_.size(), Capacity + 1);
	ASSERT_EQ(array_.capacity(), Capacity * 2);
}

TEST_F(ArrayTest, EmplaceBack)
{
	printf("Emplacing at the back\n");
	array_.emplaceBack(Capacity + 1);
	printArray(array_);

	ASSERT_TRUE(isUnmodified(array_));
	ASSERT_EQ(array_.size(), Capacity + 1);
	ASSERT_EQ(array_.capacity(), Capacity * 2);
}

TEST_F(ArrayTest, PopBack)
{
	printf("Removing at the back\n");
	array_.popBack();
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity - 1);
	ASSERT_EQ(array_.capacity(), Capacity);
	ASSERT_EQ(array_[array_.size() - 1], 8);
}

TEST_F(ArrayTest, OverwriteMiddle)
{
	printf("Overwriting in the middle\n");
	array_[2] = 22;
	printArray(array_);

	ASSERT_EQ(array_[2], 22);
	ASSERT_EQ(array_.size(), Capacity);
}

TEST_F(ArrayTest, InsertMiddle)
{
	printf("Inserting in the middle\n");
	array_.insertAt(3, 22);
	printArray(array_);

	ASSERT_EQ(array_[2], 2);
	ASSERT_EQ(array_[3], 22);
	ASSERT_EQ(array_[4], 3);
	ASSERT_EQ(array_.size(), Capacity + 1);
	ASSERT_EQ(array_.capacity(), Capacity * 2);
}

TEST_F(ArrayTest, EmplaceMiddle)
{
	printf("Emplacing in the middle\n");
	array_.emplaceAt(3, 22);
	printArray(array_);

	ASSERT_EQ(array_[2], 2);
	ASSERT_EQ(array_[3], 22);
	ASSERT_EQ(array_[4], 3);
	ASSERT_EQ(array_.size(), Capacity + 1);
	ASSERT_EQ(array_.capacity(), Capacity * 2);
}

TEST_F(ArrayTest, RemoveMiddle)
{
	printf("Removing from the middle\n");
	array_.removeAt(3);
	printArray(array_);

	ASSERT_EQ(array_[2], 2);
	ASSERT_EQ(array_[3], 4);
	ASSERT_EQ(array_[4], 5);
	ASSERT_EQ(array_.size(), Capacity - 1);
}

TEST_F(ArrayTest, UnorderedRemoveMiddle)
{
	printf("Removing from the middle (unordered)\n");
	array_.unorderedRemoveAt(3);
	printArray(array_);

	ASSERT_EQ(array_[2], 2);
	ASSERT_EQ(array_[3], 9);
	ASSERT_EQ(array_[4], 4);
	ASSERT_EQ(array_.size(), Capacity - 1);
}

TEST_F(ArrayTest, InsertFirstAndLast)
{
	printf("Inserting as first and last\n");
	array_.insertAt(0, -1);
	array_.insertAt(array_.size(), 10);
	printArray(array_);

	ASSERT_EQ(array_[0], -1);
	ASSERT_EQ(array_[1], 0);
	ASSERT_EQ(array_[9], 8);
	ASSERT_EQ(array_[10], 9);
	ASSERT_EQ(array_[11], 10);
	ASSERT_EQ(array_.size(), Capacity + 2);
	ASSERT_EQ(array_.capacity(), Capacity * 2);
}

TEST_F(ArrayTest, EmplaceFirstAndLast)
{
	printf("Emplacing as first and last\n");
	array_.emplaceAt(0, -1);
	array_.emplaceAt(array_.size(), 10);
	printArray(array_);

	ASSERT_EQ(array_[0], -1);
	ASSERT_EQ(array_[1], 0);
	ASSERT_EQ(array_[9], 8);
	ASSERT_EQ(array_[10], 9);
	ASSERT_EQ(array_[11], 10);
	ASSERT_EQ(array_.size(), Capacity + 2);
	ASSERT_EQ(array_.capacity(), Capacity * 2);
}

TEST_F(ArrayTest, RemoveFirstAndTwiceLast)
{
	printf("Removing the first and twice the last\n");
	array_.removeAt(0);
	array_.removeAt(array_.size() - 1);
	array_.removeAt(array_.size() - 1);
	printArray(array_);

	ASSERT_EQ(array_[0], 1);
	ASSERT_EQ(array_[1], 2);
	ASSERT_EQ(array_[5], 6);
	ASSERT_EQ(array_[6], 7);
	ASSERT_EQ(array_.size(), Capacity - 3);
}

TEST_F(ArrayTest, UnorderedRemoveFirstAndTwiceLast)
{
	printf("Removing the first and twice the last (unordered)\n");
	array_.unorderedRemoveAt(0);
	array_.unorderedRemoveAt(array_.size() - 1);
	array_.unorderedRemoveAt(array_.size() - 1);
	printArray(array_);

	ASSERT_EQ(array_[0], 9);
	ASSERT_EQ(array_[1], 1);
	ASSERT_EQ(array_[5], 5);
	ASSERT_EQ(array_[6], 6);
	ASSERT_EQ(array_.size(), Capacity - 3);
}

TEST_F(ArrayTest, ExtendAndShrinkSize)
{
	printf("Extending the size by two elements\n");
	const unsigned int oldSize = array_.size();
	array_.setSize(oldSize + 2);
	// No "hole" when setting this element
	array_[oldSize + 1] = 55;
	array_[oldSize] = 44;
	printArray(array_);
	ASSERT_TRUE(isUnmodified(array_));
	ASSERT_EQ(array_.size(), oldSize + 2);

	printf("Restoring previous size\n");
	array_.setSize(oldSize);
	printArray(array_);
	ASSERT_TRUE(isUnmodified(array_));
	ASSERT_EQ(array_.size(), oldSize);
}

TEST_F(ArrayTest, ExtendAndShrinkCapacity)
{
	printf("Removing the first two elements before shrinking\n");
	array_.removeAt(0);
	array_.removeAt(0);
	printArray(array_);
	ASSERT_EQ(array_[0], 2);

	const unsigned int oldCapacity = array_.capacity();
	printf("Current array capacity: %d, size:%d\n", array_.capacity(), array_.size());
	ASSERT_EQ(array_.size(), Capacity - 2);
	ASSERT_EQ(array_.capacity(), Capacity);

	array_.shrinkToFit();
	printf("New capacity after shrinkToFit(): %d, size: %d\n", array_.capacity(), array_.size());
	ASSERT_EQ(array_.size(), Capacity - 2);
	ASSERT_EQ(array_.capacity(), Capacity - 2);

	array_.setCapacity(oldCapacity);
	printf("Restoring previous capacity: %d, size: %d\n", array_.capacity(), array_.size());
	ASSERT_EQ(array_.size(), Capacity - 2);
	ASSERT_EQ(array_.capacity(), Capacity);
}

}
