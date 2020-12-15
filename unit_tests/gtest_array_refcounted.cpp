#include "gtest_array.h"
#define PRINT_ALL_COUNTERS (0)
#include "test_refcounted.h"

#define TEST_STL (0)
#if TEST_STL
	#include <vector>
#endif

int RefCounted::counter_ = 0;
int RefCounted::constructions_ = 0;
int RefCounted::destructions_ = 0;
int RefCounted::copyConstructions_ = 0;
int RefCounted::moveConstructions_ = 0;
int RefCounted::assignments_ = 0;
int RefCounted::moveAssignments_ = 0;

namespace {

class ArrayRefCountedTest : public ::testing::Test
{
  public:
	ArrayRefCountedTest()
	    : array_(Capacity) {}

  protected:
	nctl::Array<RefCounted> array_;
#if TEST_STL
	std::vector<RefCounted> stdvec_;
#endif
};

TEST_F(ArrayRefCountedTest, PushBackLValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object at the back\n");
	{
		const RefCounted rc;
		array_.pushBack(rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, PushBackRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object at the back\n");
	array_.pushBack(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, PushAndPopBack)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object at the back\n");
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Removing the RefCounted object from the back\n");
	array_.popBack();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(ArrayRefCountedTest, DoublePushBackClear)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting two new RefCounted objects at the back\n");
	array_.pushBack(RefCounted());
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Clearing the array\n");
	array_.clear();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(ArrayRefCountedTest, EmplaceBack)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object at the back\n");
	array_.emplaceBack(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, InsertLValueAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a RefCounted object at the back\n");
	{
		const RefCounted rc;
		array_.insert(array_.end(), rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, InsertRValueAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a RefCounted object at the back\n");
	array_.insert(array_.end(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, InsertLValueAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printf("Inserting a RefCounted object at the beginning\n");
	{
		const RefCounted rc;
		array_.insert(array_.begin(), rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, InsertRValueAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printf("Move inserting a RefCounted object at the beginning\n");
	array_.insert(array_.begin(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, EmplaceAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object at the back\n");
	array_.emplace(array_.end(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, EmplaceAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printf("Emplacing a RefCounted object at the beginning\n");
	array_.emplace(array_.begin(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, CopyConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new array with copy construction\n");
	nctl::Array<RefCounted> newArray(array_);
	printRefCounters();

	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(newArray.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, MoveConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new array with move construction\n");
	nctl::Array<RefCounted> newArray(nctl::move(array_));
	printRefCounters();

	ASSERT_EQ(array_.size(), 0);
	ASSERT_EQ(newArray.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, CopyAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new array with the copy assignment operator\n");
	nctl::Array<RefCounted> newArray;
	newArray = array_;
	printRefCounters();

	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(newArray.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, CopyAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new array with the copy assignment operator\n");
	nctl::Array<RefCounted> newArray;
	newArray = array_;
	printRefCounters();

	ASSERT_EQ(array_.size(), 2);
	ASSERT_EQ(newArray.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 4);
}

TEST_F(ArrayRefCountedTest, CopyAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new array with the copy assignment operator\n");
	nctl::Array<RefCounted> newArray;
	newArray.pushBack(RefCounted());
	newArray.pushBack(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newArray = array_;
	printRefCounters();

	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(newArray.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, MoveAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new array with the move assignment operator\n");
	nctl::Array<RefCounted> newArray;
	newArray = nctl::move(array_);
	printRefCounters();

	ASSERT_EQ(array_.size(), 0);
	ASSERT_EQ(newArray.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, MoveAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new array with the move assignment operator\n");
	nctl::Array<RefCounted> newArray;
	newArray = nctl::move(array_);
	printRefCounters();

	ASSERT_EQ(array_.size(), 0);
	ASSERT_EQ(newArray.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, MoveAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new array with the move assignment operator\n");
	nctl::Array<RefCounted> newArray;
	newArray.pushBack(RefCounted());
	newArray.pushBack(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newArray = nctl::move(array_);
	printRefCounters();

	ASSERT_EQ(array_.size(), 0);
	ASSERT_EQ(newArray.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, RemoveLastWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Insert three elements)\n");
	array_.pushBack(RefCounted());
	array_.pushBack(RefCounted());
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 3);

	printf("Removing last element\n");
	array_.erase(array_.end() - 1);
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, RemoveSecondHalfWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill the whole array up to capacity (%u elements)\n", Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);

	printf("Removing the second half of the array\n");
	array_.erase(array_.begin() + array_.size() / 2, array_.end());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity / 2);
}

TEST_F(ArrayRefCountedTest, UnorderedRemoveSecondHalfWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill the whole array up to capacity (%u elements)\n", Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);

	printf("Removing the first half of the array (unordered)\n");
	array_.unorderedErase(array_.begin(), array_.begin() + array_.size() / 2);
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity / 2);
}

TEST_F(ArrayRefCountedTest, SetHalfSize)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill the whole array up to capacity (%u elements)\n", Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);

	printf("Shrink the array to half its size\n");
	array_.setSize(Capacity / 2);
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity / 2);
}

TEST_F(ArrayRefCountedTest, SetDoubleSize)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill half the array (%u elements)\n", Capacity / 2);
	for (unsigned int i = 0; i < Capacity / 2; i++)
		array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity / 2);

	printf("Extend the array to double its size\n");
	array_.setSize(Capacity);
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);
}

TEST_F(ArrayRefCountedTest, SetHalfCapacity)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill the whole array up to capacity (%u elements)\n", Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);

	printf("Shrink the array to half its capacity\n");
	array_.setCapacity(Capacity / 2);
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity / 2);
}

TEST_F(ArrayRefCountedTest, SetDoubleCapacity)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill the whole array up to capacity (%u elements)\n", Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);

	printf("Extend the array to double its capacity\n");
	array_.setCapacity(Capacity * 2);
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);
}

#if TEST_STL
TEST_F(ArrayRefCountedTest, StdVector_PushBackLValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object at the back\n");
	{
		const RefCounted rc;
		stdvec_.push_back(rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, StdVector_PushBackRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object at the back\n");
	stdvec_.push_back(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, StdVector_PushAndPopBack)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object at the back\n");
	stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Removing the RefCounted object from the back\n");
	stdvec_.pop_back();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(ArrayRefCountedTest, StdVector_DoublePushBackClear)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting two new RefCounted objects at the back\n");
	stdvec_.push_back(RefCounted());
	stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Clearing the array\n");
	stdvec_.clear();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(ArrayRefCountedTest, StdVector_EmplaceBack)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object at the back\n");
	stdvec_.emplace_back(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, StdVector_InsertLValueAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a RefCounted object at the back\n");
	{
		const RefCounted rc;
		stdvec_.insert(stdvec_.end(), rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, StdVector_InsertRValueAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a RefCounted object at the back\n");
	stdvec_.insert(stdvec_.end(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, StdVector_InsertLValueAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdvec_.push_back(RefCounted());
	printf("Inserting a RefCounted object at the beginning\n");
	{
		const RefCounted rc;
		stdvec_.insert(stdvec_.begin(), rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, StdVector_InsertRValueAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdvec_.push_back(RefCounted());
	printf("Move inserting a RefCounted object at the beginning\n");
	stdvec_.insert(stdvec_.begin(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, StdVector_EmplaceAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object at the back\n");
	stdvec_.emplace(stdvec_.end(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, StdVector_EmplaceAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdvec_.push_back(RefCounted());
	printf("Emplacing a RefCounted object at the beginning\n");
	stdvec_.emplace(stdvec_.begin(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, StdVector_CopyConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new vector with copy construction\n");
	std::vector<RefCounted> newVector(stdvec_);
	printRefCounters();

	ASSERT_EQ(stdvec_.size(), 1);
	ASSERT_EQ(newVector.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, StdVector_MoveConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new vector with move construction\n");
	std::vector<RefCounted> newVector(std::move(stdvec_));
	printRefCounters();

	ASSERT_EQ(stdvec_.size(), 0);
	ASSERT_EQ(newVector.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, StdVector_CopyAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new vector with the copy assignment operator\n");
	std::vector<RefCounted> newVector;
	newVector = stdvec_;
	printRefCounters();

	ASSERT_EQ(stdvec_.size(), 1);
	ASSERT_EQ(newVector.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, StdVector_CopyAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdvec_.push_back(RefCounted());
	stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new vector with the copy assignment operator\n");
	std::vector<RefCounted> newVector;
	newVector = stdvec_;
	printRefCounters();

	ASSERT_EQ(stdvec_.size(), 2);
	ASSERT_EQ(newVector.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 4);
}

TEST_F(ArrayRefCountedTest, StdVector_CopyAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new vector with the copy assignment operator\n");
	std::vector<RefCounted> newVector;
	newVector.push_back(RefCounted());
	newVector.push_back(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newVector = stdvec_;
	printRefCounters();

	ASSERT_EQ(stdvec_.size(), 1);
	ASSERT_EQ(newVector.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, StdVector_MoveAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new vector with the move assignment operator\n");
	std::vector<RefCounted> newVector;
	newVector = std::move(stdvec_);
	printRefCounters();

	ASSERT_EQ(stdvec_.size(), 0);
	ASSERT_EQ(newVector.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, StdVector_MoveAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdvec_.push_back(RefCounted());
	stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new vector with the move assignment operator\n");
	std::vector<RefCounted> newVector;
	newVector = std::move(stdvec_);
	printRefCounters();

	ASSERT_EQ(stdvec_.size(), 0);
	ASSERT_EQ(newVector.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, StdVector_MoveAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new vector with the move assignment operator\n");
	std::vector<RefCounted> newVector;
	newVector.push_back(RefCounted());
	newVector.push_back(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newVector = std::move(stdvec_);
	printRefCounters();

	ASSERT_EQ(stdvec_.size(), 0);
	ASSERT_EQ(newVector.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ArrayRefCountedTest, StdVector_RemoveLastWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Insert three elements)\n");
	stdvec_.push_back(RefCounted());
	stdvec_.push_back(RefCounted());
	stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 3);

	printf("Removing last element\n");
	stdvec_.erase(stdvec_.end());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ArrayRefCountedTest, StdVector_RemoveSecondHalfWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill the whole array up to capacity (%u elements)\n", Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);

	printf("Removing the second half of the array\n");
	stdvec_.erase(stdvec_.begin() + stdvec_.size() / 2, stdvec_.end());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity / 2);
}

TEST_F(ArrayRefCountedTest, StdVector_ResizeHalf)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill the whole array up to capacity (%u elements)\n", Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);

	printf("Shrink the array to half its size\n");
	stdvec_.resize(Capacity / 2);
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity / 2);
}

TEST_F(ArrayRefCountedTest, StdVector_ResizeDouble)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill half the array (%u elements)\n", Capacity / 2);
	for (unsigned int i = 0; i < Capacity / 2; i++)
		stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity / 2);

	printf("Extend the array to double its size\n");
	stdvec_.resize(Capacity);
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);
}

TEST_F(ArrayRefCountedTest, StdVector_ShrinkToFit)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill half the array (%u elements)\n", Capacity / 2);
	for (unsigned int i = 0; i < Capacity / 2; i++)
		stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity / 2);

	printf("Shrink the array to its size\n");
	stdvec_.shrink_to_fit();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity / 2);
}

TEST_F(ArrayRefCountedTest, StdVector_ReserveDouble)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill the whole array up to capacity (%u elements)\n", Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		stdvec_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);

	printf("Extend the array to double its capacity\n");
	stdvec_.reserve(Capacity * 2);
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);
}

#endif

}
