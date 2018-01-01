#include "gtest_list.h"
#include "test_functions.h"

namespace {

class ListAlgorithmsTest : public ::testing::Test
{
  public:
	ListAlgorithmsTest() { }

  protected:
	void SetUp() { initList(list_); }

	nc::List<int> list_;
};

TEST_F(ListAlgorithmsTest, Minimum)
{
	const int minimum = *nc::minElement(list_.begin(), list_.end());
	printf("Minimum element: %d\n", minimum);

	ASSERT_EQ(minimum, FirstElement);
}

TEST_F(ListAlgorithmsTest, Maximum)
{
	const int maximum = *nc::maxElement(list_.begin(), list_.end());
	printf("Maximum element: %d\n", maximum);

	ASSERT_EQ(maximum, LastElement);
}

TEST_F(ListAlgorithmsTest, AllOfGreater)
{
	const bool allOf = nc::allOf(list_.begin(), list_.end(), nc::IsGreaterThan<int>(-1));
	printf("All bigger than -1: %d\n", allOf);
	ASSERT_EQ(allOf, true);
}

TEST_F(ListAlgorithmsTest, NoneOfGreater)
{
	const bool noneOf = nc::noneOf(list_.begin(), list_.end(), nc::IsGreaterThan<int>(5));
	printf("No one bigger than 5: %d\n", noneOf);
	ASSERT_EQ(noneOf, false);
}

TEST_F(ListAlgorithmsTest, AnyOfGreater)
{
	const bool anyOf = nc::anyOf(list_.begin(), list_.end(), nc::IsGreaterThan<int>(5));
	printf("Anyone bigger than 5: %d\n", anyOf);
	ASSERT_EQ(anyOf, true);
}

TEST_F(ListAlgorithmsTest, AddValueForEach)
{
	const int value = 10;
	printf("Adding %d to each element of the list\n", value);
	nc::forEach(list_.begin(), list_.end(), addValue<value>);
	printList(list_);

	ASSERT_EQ(list_.size(), Length);
	int n = FirstElement;
	for (nc::List<int>::ConstIterator i = list_.begin(); i != list_.end(); ++i)
	{
		ASSERT_EQ(*i, n + value);
		n++;
	}
}

TEST_F(ListAlgorithmsTest, CountEqual)
{
	const int counter = nc::count(list_.begin(), list_.end(), 6);
	printf("Number of elements equal to 6: %d\n", counter);
	ASSERT_EQ(counter, 1);
}

TEST_F(ListAlgorithmsTest, CountElementsGreater)
{
	const int counter = nc::countIf(list_.begin(), list_.end(), nc::IsGreaterThan<int>(4));
	printf("Number of elements bigger than 4: %d\n", counter);
	ASSERT_EQ(counter, 6);
}

TEST_F(ListAlgorithmsTest, DistanceToFirstElementEqual)
{
	const int position = nc::distance(list_.begin(), nc::find(list_.begin(), list_.end(), 3));
	printf("First element equal to 3 in position: %d\n", position);
	ASSERT_EQ(position, 3);
}

TEST_F(ListAlgorithmsTest, DistanceToFirstElementBigger)
{
	const int counter = nc::distance(list_.begin(), nc::findIf(list_.begin(), list_.end(), nc::IsGreaterThan<int>(3)));
	printf("First element bigger than 3 in position: %d\n", counter);
	ASSERT_EQ(counter, 4);
}

TEST_F(ListAlgorithmsTest, DistanceToFirstElementNotBigger)
{
	const int counter = nc::distance(list_.begin(), nc::findIfNot(list_.begin(), list_.end(), nc::IsGreaterThan<int>(3)));
	printf("First element not bigger than 3 in position: %d\n", counter);
	ASSERT_EQ(counter, 0);
}

TEST_F(ListAlgorithmsTest, CheckEqual)
{
	printf("Copying the list to a second one and check they are equal\n");
	nc::List<int> newList;
	newList = list_;
	printList(newList);

	ASSERT_TRUE(nc::equal(list_.begin(), list_.end(), newList.begin()));
	assertListsAreEqual(list_, newList);
}

TEST_F(ListAlgorithmsTest, FillNElements)
{
	printf("Filling half list with zeroes\n");
	nc::fillN(list_.begin(), Length / 2, 0);
	printList(list_);

	ASSERT_EQ(list_.size(), Length);
	const unsigned int halfSize = list_.size() / 2;
	nc::List<int>::ConstIterator halfIt = nc::next(list_.begin(), halfSize);

	for (nc::List<int>::ConstIterator i = list_.begin(); i != halfIt; ++i)
		ASSERT_EQ(*i, 0);

	int n = static_cast<int>(halfSize);
	for (nc::List<int>::ConstIterator i = halfIt; i != list_.end(); ++i)
	{
		ASSERT_EQ(*i, n);
		n++;
	}
}

TEST_F(ListAlgorithmsTest, FillWithIterators)
{
	printf("Filling the whole list with zeroes\n");
	nc::fill(list_.begin(), list_.end(), 0);
	printList(list_);

	ASSERT_EQ(list_.size(), Length);
	for (nc::List<int>::ConstIterator i = list_.begin(); i != list_.end(); ++i)
		ASSERT_EQ(*i, 0);
}

TEST_F(ListAlgorithmsTest, ClampElements)
{
	const int minValue = 3;
	const int maxValue = 6;

	printf("Clamping list nodes between %d and %d\n", minValue, maxValue);
	nc::clampElements(list_.begin(), list_.end(), minValue, maxValue);
	printList(list_);

	ASSERT_EQ(list_.size(), Length);
	for (nc::List<int>::ConstIterator i = list_.begin(); i != list_.end(); ++i)
	{
		ASSERT_TRUE(*i >= minValue);
		ASSERT_TRUE(*i <= maxValue);
	}
}


TEST_F(ListAlgorithmsTest, Replace)
{
	const int oldValue = 5;
	const int newValue = 55;

	printf("Replacing all nodes equal to %d with %d\n", oldValue, newValue);
	nc::replace(list_.begin(), list_.end(), oldValue, newValue);
	printList(list_);

	ASSERT_EQ(list_.size(), Length);
	nc::List<int>::ConstIterator i = nc::next(list_.begin(), oldValue);
	ASSERT_EQ(*i, newValue);
}

TEST_F(ListAlgorithmsTest, ReplaceIf)
{
	const int refValue = 5;
	const int newValue = 55;

	printf("Replacing all nodes bigger than %d with %d\n", refValue, newValue);
	nc::replaceIf(list_.begin(), list_.end(), nc::IsEqualTo<int>(refValue), newValue);
	printList(list_);

	ASSERT_EQ(list_.size(), Length);
	nc::List<int>::ConstIterator i = nc::next(list_.begin(), refValue);
	ASSERT_EQ(*i, newValue);
}

TEST_F(ListAlgorithmsTest, Generate)
{
	const int value = 1;
	printf("Generating a sequence starting at %d and store it into the list\n", value);
	nc::generate(list_.begin(), list_.end(), GenerateSequenceFrom(value));
	printList(list_);

	ASSERT_EQ(list_.size(), Length);
	int n = FirstElement;
	for (nc::List<int>::ConstIterator i = list_.begin(); i != list_.end(); ++i)
	{
		ASSERT_EQ(*i, n + value);
		n++;
	}
}

TEST_F(ListAlgorithmsTest, GenerateN)
{
	const int value = -4;
	const int numElements = 5;
	printf("Generating a sequence of %d values starting at %d and store it into the list\n", numElements, value);
	nc::generateN(list_.begin(), numElements, GenerateSequenceFrom(value));
	printList(list_);

	ASSERT_EQ(list_.size(), Length);
	nc::List<int>::ConstIterator numElementsIt = nc::next(list_.begin(), numElements);
	int n = FirstElement;
	for (nc::List<int>::ConstIterator i = list_.begin(); i != numElementsIt; ++i)
	{
		ASSERT_EQ(*i, n + value);
		n++;
	}
	for (nc::List<int>::ConstIterator i = numElementsIt; i != list_.end(); ++i)
	{
		ASSERT_EQ(*i, n);
		n++;
	}
}

TEST_F(ListAlgorithmsTest, SortedUntil)
{
	const unsigned int position = 5;
	const int element = -1;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	nc::List<int>::Iterator positionIt = nc::next(list_.begin(), position);
	*positionIt = element;
	printList(list_);

	nc::List<int>::ConstIterator unsorted = nc::isSortedUntil(list_.begin(), list_.end());
	printf("The list is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(ListAlgorithmsTest, ReverseSortedUntil)
{
	printf("Clearing the list and adding nodes at reverse\n");
	initListReverse(list_);
	printList(list_);

	const unsigned int position = 5;
	const int element = 99;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	nc::List<int>::Iterator positionIt = nc::next(list_.begin(), position);
	*positionIt = element;
	printList(list_);

	nc::List<int>::ConstIterator unsorted = nc::isSortedUntil(list_.begin(), list_.end(), nc::IsGreater<int>);
	printf("The list is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

}
