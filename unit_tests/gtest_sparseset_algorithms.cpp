#include "gtest_sparseset.h"
#include "test_functions.h"

namespace {

class SparseSetAlgorithmsTest : public ::testing::Test
{
  public:
	SparseSetAlgorithmsTest()
	    : sparseset_(Capacity, MaxValue) {}

  protected:
	void SetUp() override { initSparseSet(sparseset_); }

	SparseSetTestType sparseset_;
};

TEST_F(SparseSetAlgorithmsTest, Minimum)
{
	const int minimum = *nctl::minElement(sparseset_.begin(), sparseset_.end());
	printf("Minimum element: %d\n", minimum);

	ASSERT_EQ(minimum, 0);
}

TEST_F(SparseSetAlgorithmsTest, Maximum)
{
	const int maximum = *nctl::maxElement(sparseset_.begin(), sparseset_.end());
	printf("Maximum element: %d\n", maximum);

	ASSERT_EQ(maximum, Size - 1);
}

TEST_F(SparseSetAlgorithmsTest, AllOfGreater)
{
	const bool allOf = nctl::allOf(sparseset_.begin(), sparseset_.end(), nctl::IsGreaterThan<int>(-1));
	printf("All bigger than -1: %d\n", allOf);
	ASSERT_EQ(allOf, true);
}

TEST_F(SparseSetAlgorithmsTest, NoneOfGreater)
{
	const bool noneOf = nctl::noneOf(sparseset_.begin(), sparseset_.end(), nctl::IsGreaterThan<int>(5));
	printf("No one bigger than 5: %d\n", noneOf);
	ASSERT_EQ(noneOf, false);
}

TEST_F(SparseSetAlgorithmsTest, AnyOfGreater)
{
	const bool anyOf = nctl::anyOf(sparseset_.begin(), sparseset_.end(), nctl::IsGreaterThan<int>(5));
	printf("Anyone bigger than 5: %d\n", anyOf);
	ASSERT_EQ(anyOf, true);
}

TEST_F(SparseSetAlgorithmsTest, CountEqual)
{
	const int counter = nctl::count(sparseset_.begin(), sparseset_.end(), 6);
	printf("Number of elements equal to 6: %d\n", counter);
	ASSERT_EQ(counter, 1);
}

TEST_F(SparseSetAlgorithmsTest, CountElementsGreater)
{
	const int counter = nctl::countIf(sparseset_.begin(), sparseset_.end(), nctl::IsGreaterThan<int>(4));
	printf("Number of elements bigger than 4: %d\n", counter);
	ASSERT_EQ(counter, 3);
}

TEST_F(SparseSetAlgorithmsTest, DistanceToFirstElementEqual)
{
	const int position = nctl::distance(sparseset_.begin(), nctl::find(sparseset_.begin(), sparseset_.end(), 3));
	printf("First element equal to 3 in position: %d\n", position);
	ASSERT_EQ(position, 3);
}

TEST_F(SparseSetAlgorithmsTest, DistanceToFirstElementBigger)
{
	const int counter = nctl::distance(sparseset_.begin(), nctl::findIf(sparseset_.begin(), sparseset_.end(), nctl::IsGreaterThan<int>(3)));
	printf("First element bigger than 3 in position: %d\n", counter);
	ASSERT_EQ(counter, 4);
}

TEST_F(SparseSetAlgorithmsTest, DistanceToFirstElementNotBigger)
{
	const int counter = nctl::distance(sparseset_.begin(), nctl::findIfNot(sparseset_.begin(), sparseset_.end(), nctl::IsGreaterThan<int>(3)));
	printf("First element not bigger than 3 in position: %d\n", counter);
	ASSERT_EQ(counter, 0);
}

TEST_F(SparseSetAlgorithmsTest, CheckEqual)
{
	printf("Copying the sparseset to a second one and check they are equal\n");
	SparseSetTestType newSparseset(Capacity, MaxValue);
	newSparseset = sparseset_;
	printSparseSet(newSparseset);

	ASSERT_TRUE(nctl::equal(sparseset_.begin(), sparseset_.end(), newSparseset.begin()));
	assertSparseSetsAreEqual(sparseset_, newSparseset);
}

}
