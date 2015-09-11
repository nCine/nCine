#include "test_functions.h"
#include "algorithms.h"

int main(int argc, char **argv)
{
	const int Size = 10;
	nc::List<int> list;

	printf("Inserting some elements to the back\n");
	for (int i = 0; i < Size; i++)
	{
		list.pushBack(i);
	}
	printList(list);

	int minimum = *nc::minElement(list.begin(), list.end());
	printf("Minimum element: %d\n", minimum);

	int maximum = *nc::maxElement(list.begin(), list.end());
	printf("Maximum element: %d\n", maximum);

	printf("All bigger than -1: %d\n", nc::allOf(list.begin(), list.end(), nc::IsGreaterThan<int>(-1)));
	printf("No one bigger than 5: %d\n", nc::noneOf(list.begin(), list.end(), nc::IsGreaterThan<int>(5)));
	printf("Anyone bigger than 5: %d\n", nc::anyOf(list.begin(), list.end(), nc::IsGreaterThan<int>(5)));

	printf("Adding 10 to each element of the list\n");
	nc::forEach(list.begin(), list.end(), addValue<10>);
	printList(list);

	minimum = *nc::minElement(list.begin(), list.end());
	printf("Minimum element: %d\n", minimum);

	maximum = *nc::maxElement(list.begin(), list.end());
	printf("Maximum element: %d\n", maximum);

	int counter = nc::count(list.begin(), list.end(), 16);
	printf("Number of elements equal to 16: %d\n", counter);

	counter = nc::countIf(list.begin(), list.end(), nc::IsGreaterThan<int>(14));
	printf("Number of elements bigger than 14: %d\n", counter);

	printf("First element equal to 13 in position: %d\n", nc::distance(list.begin(), nc::find(list.begin(), list.end(), 13)));
	printf("First element bigger than 13 in position: %d\n", nc::distance(list.begin(), nc::findIf(list.begin(), list.end(), nc::IsGreaterThan<int>(13))));
	printf("First element not bigger than 13 in position: %d\n", nc::distance(list.begin(), nc::findIfNot(list.begin(), list.end(), nc::IsGreaterThan<int>(13))));

	printf("Fill half list with zeroes:\n");
	nc::fillN(list.begin(), Size/2, 0);
	printList(list);

	printf("Fill the whole list with zeroes:\n");
	nc::fill(list.begin(), list.end(), 0);
	printList(list);

	printf("Creating a second list with random elements\n");
	nc::List<int> list2;
	for (int i = 0; i < Size; i++)
	{
		list2.pushBack(rand() % 100);
	}
	printList(list2);

	printf("Storing transformed random numbers back into the first list\n");
	nc::transform(list2.begin(), list2.end(), list.begin(), copyAddValue<10>);
	printList(list);

	return EXIT_SUCCESS;
}
