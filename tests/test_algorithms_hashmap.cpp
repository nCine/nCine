#include "HashMap.h"
#include "HashMapIterator.h"
#include "algorithms.h"
#include "test_functions.h"

namespace {

inline void printHashmap(const nc::HashMap<int, int, nc::FixedHashFunc<int> >& hashmap)
{
	unsigned int index = 0;

	for (nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = hashmap.begin(); i != hashmap.end(); ++i)
	{
		printf("[%u]=%d ", index, *i);
		index++;
	}
	printf("\n");
}

}

int main(int argc, char **argv)
{
	const int Size = 10;
	nc::HashMap<int, int, nc::FixedHashFunc<int> > hashmap(32);

	printf("Inserting some elements\n");
	for (int i = 0; i < Size; i++)
	{
		hashmap[i] = i;
	}

	for (nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = hashmap.begin(); i != hashmap.end(); ++i)
	{
		printf("hash: %lu, key: %d, value: %d\n", i.hash(), i.key(), i.value());
	}

	int minimum = *nc::minElement(hashmap.begin(), hashmap.end());
	printf("Minimum element: %d\n", minimum);

	int maximum = *nc::maxElement(hashmap.begin(), hashmap.end());
	printf("Maximum element: %d\n", maximum);

	printf("All bigger than -1: %d\n", nc::allOf(hashmap.begin(), hashmap.end(), nc::IsGreaterThan<int>(-1)));
	printf("No one bigger than 5: %d\n", nc::noneOf(hashmap.begin(), hashmap.end(), nc::IsGreaterThan<int>(5)));
	printf("Anyone bigger than 5: %d\n", nc::anyOf(hashmap.begin(), hashmap.end(), nc::IsGreaterThan<int>(5)));

	printf("Adding 10 to each element of the hashmap\n");
	nc::forEach(hashmap.begin(), hashmap.end(), addValue<10>);
	printHashmap(hashmap);

	minimum = *nc::minElement(hashmap.begin(), hashmap.end());
	printf("Minimum element: %d\n", minimum);

	maximum = *nc::maxElement(hashmap.begin(), hashmap.end());
	printf("Maximum element: %d\n", maximum);

	int counter = nc::count(hashmap.begin(), hashmap.end(), 16);
	printf("Number of elements equal to 16: %d\n", counter);

	counter = nc::countIf(hashmap.begin(), hashmap.end(), nc::IsGreaterThan<int>(14));
	printf("Number of elements bigger than 14: %d\n", counter);

	printf("First element equal to 13 in position: %d\n", nc::distance(hashmap.begin(), nc::find(hashmap.begin(), hashmap.end(), 13)));
	printf("First element bigger than 13 in position: %d\n", nc::distance(hashmap.begin(), nc::findIf(hashmap.begin(), hashmap.end(), nc::IsGreaterThan<int>(13))));
	printf("First element not bigger than 13 in position: %d\n", nc::distance(hashmap.begin(), nc::findIfNot(hashmap.begin(), hashmap.end(), nc::IsGreaterThan<int>(13))));

	printf("Fill half hashmap with zeroes:\n");
	nc::fillN(hashmap.begin(), Size/2, 0);
	printHashmap(hashmap);

	printf("Fill the whole hashmap with zeroes:\n");
	nc::fill(hashmap.begin(), hashmap.end(), 0);
	printHashmap(hashmap);

	return EXIT_SUCCESS;
}
