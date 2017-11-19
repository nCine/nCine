#include "gtest_staticarray.h"

namespace {

class StaticArrayTest : public ::testing::Test
{
  protected:
	void SetUp() override { initArray(array_); }

	nc::StaticArray<int, Capacity> array_;
};

TEST(StaticArrayDeathTest, AccessBeyondSize)
{
	printf("Trying to access an element within capacity but beyond size\n");
	nc::StaticArray<int, Capacity> array;
	array[0] = 0;

	ASSERT_DEATH(array.at(5) = 1, "");
}

TEST(StaticArrayDeathTest, AccessConstAtSize)
{
	printf("Trying to access an element of a const array at size\n");
	nc::StaticArray<int, Capacity> array;
	array[0] = 0;

	const nc::StaticArray<int, Capacity> &constArray = array;

	ASSERT_DEATH(constArray.at(1), "");
}

TEST_F(StaticArrayTest, AccessEmptyWithinSize)
{
	printf("Accessing at array size to add elements\n");
	nc::StaticArray<int, Capacity> newArray;
	for (unsigned int i = 0; i < Capacity; i++)
		newArray.at(i) = i;

	ASSERT_EQ(newArray.size(), Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(newArray.at(i), static_cast<int>(i));
}

TEST_F(StaticArrayTest, AccessWithinSize)
{
	printf("Accessing an element within the array size\n");

	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(array_.at(i), static_cast<int>(i));
}

TEST_F(StaticArrayTest, AccessWithPointer)
{
	printf("Accessing an array element with a pointer\n");

	int *ptr = array_.data();
	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(ptr[i], static_cast<int>(i));
}

TEST_F(StaticArrayTest, SubscriptAccessConstAtSize)
{
	nc::StaticArray<int, Capacity> newArray;
	newArray[0] = 0;

	const nc::StaticArray<int, Capacity> &constArray = newArray;
	const int value = constArray[1];
	printf("Trying to access an element of a const array at size: %d\n", value);

	ASSERT_EQ(newArray.capacity(), Capacity);
	ASSERT_EQ(newArray.size(), 1);
}

TEST_F(StaticArrayTest, SubscriptAccessEmptyWithinSize)
{
	printf("Accessing at array size to add elements\n");
	nc::StaticArray<int, Capacity> newArray;
	for (unsigned int i = 0; i < Capacity; i++)
		newArray[i] = static_cast<int>(i);
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(newArray.at(i), static_cast<int>(i));
}

TEST_F(StaticArrayTest, SubscriptAccessNotGrowing)
{
	printf("Trying to access an element beyond the capacity of a static array\n");
	array_[Capacity] = Capacity;

	ASSERT_EQ(array_.capacity(), Capacity);
	ASSERT_EQ(array_.size(), Capacity);
}

TEST_F(StaticArrayTest, SetSizeOnStatic)
{
	printf("Trying to extend the size of a static array\n");
	nc::StaticArray<int, 1> array;
	array[0] = 0;
	array_.setSize(Capacity);

	ASSERT_EQ(array.capacity(), 1);
	ASSERT_EQ(array.size(), 1);
}

TEST_F(StaticArrayTest, FrontElement)
{
	const int front = array_.front();
	printf("Retrieving the front element: %d\n", front);

	ASSERT_TRUE(isUnmodified(array_));
	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_EQ(array_.capacity(), Capacity);
	ASSERT_EQ(front, array_[0]);
}

TEST_F(StaticArrayTest, BackElement)
{
	const int back = array_.back();
	printf("Retrieving the back element: %d\n", back);

	ASSERT_TRUE(isUnmodified(array_));
	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_EQ(array_.capacity(), Capacity);
	ASSERT_EQ(back, array_[array_.size() - 1]);
}

TEST_F(StaticArrayTest, OverwriteMiddle)
{
	printf("Overwriting in the middle\n");
	array_[2] = 22;
	printArray(array_);

	ASSERT_EQ(array_[2], 22);
	ASSERT_EQ(array_.size(), Capacity);
}

TEST_F(StaticArrayTest, ShrinkAndExtendSize)
{
	printf("Shriking the size by two elements\n");
	const unsigned int oldSize = array_.size();
	array_.setSize(oldSize - 2);
	printArray(array_);
	ASSERT_EQ(array_.size(), oldSize - 2);

	printf("Restoring previous size\n");
	array_.setSize(oldSize);
	printArray(array_);
	ASSERT_TRUE(isUnmodified(array_));
	ASSERT_EQ(array_.size(), oldSize);
}

}
