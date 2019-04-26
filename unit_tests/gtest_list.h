#ifndef GTEST_LIST_H
#define GTEST_LIST_H

#include <nctl/algorithms.h>
#include <nctl/List.h>
#include <ncine/Random.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

const unsigned int Length = 11;
const int FirstElement = 0;
const int LastElement = 10;

void printList(const nctl::List<int> &list)
{
	unsigned int index = 0;

	printf("Size %d: ", list.size());
	for (nctl::List<int>::ConstIterator i = list.begin(); i != list.end(); ++i)
	{
		printf("[%u]=%d ", index, *i);
		index++;
	}
	printf("\n");
}

void printList(const char *message, const nctl::List<int> &list)
{
	printf("%s ", message);
	printList(list);
}

void initList(nctl::List<int> &list)
{
	list.clear();

	int value = FirstElement;
	for (unsigned int i = 0; i < Length; i++)
		list.pushBack(value++);
}

void initListReverse(nctl::List<int> &list)
{
	list.clear();

	int value = LastElement;
	for (unsigned int i = 0; i < Length; i++)
		list.pushBack(value--);
}

void initListRandom(nctl::List<int> &list)
{
	list.clear();

	for (unsigned int i = 0; i < Length; i++)
		list.pushBack(nc::random().integer(0, 100));
}

bool isUnmodified(const nctl::List<int> &list)
{
	int value = FirstElement;

	for (nctl::List<int>::ConstIterator i = list.begin(); i != list.end(); ++i)
	{
		if (*i != value)
			return false;

		value++;
	}

	return true;
}

unsigned int calcLength(const nctl::List<int> &list)
{
	unsigned int length = 0;

	for (nctl::List<int>::ConstIterator i = list.begin(); i != list.end(); ++i)
		length++;

	return length;
}

void assertListMatchesArray(const nctl::List<int> &list, int *array)
{
	for (nctl::List<int>::ConstIterator i = list.begin(); i != list.end(); ++i)
	{
		ASSERT_EQ(*i, *array);
		array++;
	}
}

void assertListsAreEqual(const nctl::List<int> &list1, const nctl::List<int> &list2)
{
	ASSERT_EQ(list1.size(), list2.size());

	nctl::List<int>::ConstIterator list1It = list1.begin();
	nctl::List<int>::ConstIterator list2It = list2.begin();
	while (list1It != list1.end())
	{
		ASSERT_EQ(*list1It, *list2It);

		list1It++;
		list2It++;
	}
}

bool isSorted(const nctl::List<int> &list)
{
	nctl::List<int>::ConstIterator currentNode = list.begin();
	nctl::List<int>::ConstIterator nextNode = list.begin()++;

	while (nextNode != list.end())
	{
		if (*currentNode > *nextNode)
			return false;

		currentNode++;
		nextNode++;
	}

	return true;
}

bool isReverseSorted(const nctl::List<int> &list)
{
	nctl::List<int>::ConstIterator currentNode = list.begin();
	nctl::List<int>::ConstIterator nextNode = list.begin()++;

	while (nextNode != list.end())
	{
		if (*currentNode < *nextNode)
			return false;

		currentNode++;
		nextNode++;
	}

	return true;
}

bool isOdd(int n)
{
	return (n % 2 != 0);
}

bool isEven(int n)
{
	return (n % 2 == 0);
}

}

#endif
