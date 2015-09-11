#include "test_functions.h"
#include "algorithms.h"

int main(int argc, char **argv)
{
	const unsigned int Capacity = 10;
	nc::Array<int> array(Capacity);

	printf("Filling the whole array\n");
	for (unsigned int i = 0; i < Capacity; i++)
	{
		array[i] = i;
	}
	printArray(array);

	int minimum = *nc::minElement(array.begin(), array.end());
	printf("Minimum element: %d\n", minimum);

	int maximum = *nc::maxElement(array.begin(), array.end());
	printf("Maximum element: %d\n", maximum);

	printf("All bigger than -1: %d\n", nc::allOf(array.begin(), array.end(), nc::IsGreaterThan<int>(-1)));
	printf("No one bigger than 5: %d\n", nc::noneOf(array.begin(), array.end(), nc::IsGreaterThan<int>(5)));
	printf("Anyone bigger than 5: %d\n", nc::anyOf(array.begin(), array.end(), nc::IsGreaterThan<int>(5)));

	printf("Adding 10 to each element of the array\n");
	nc::forEach(array.begin(), array.end(), addValue<10>);
	printArray(array);

	minimum = *nc::minElement(array.begin(), array.end());
	printf("Minimum element: %d\n", minimum);

	maximum = *nc::maxElement(array.begin(), array.end());
	printf("Maximum element: %d\n", maximum);

	int counter = nc::count(array.begin(), array.end(), 16);
	printf("Number of elements equal to 16: %d\n", counter);

	counter = nc::countIf(array.begin(), array.end(), nc::IsGreaterThan<int>(14));
	printf("Number of elements bigger than 14: %d\n", counter);

	printf("First element equal to 13 in position: %d\n", nc::distance(array.begin(), nc::find(array.begin(), array.end(), 13)));
	printf("First element bigger than 13 in position: %d\n", nc::distance(array.begin(), nc::findIf(array.begin(), array.end(), nc::IsGreaterThan<int>(13))));
	printf("First element not bigger than 13 in position: %d\n", nc::distance(array.begin(), nc::findIfNot(array.begin(), array.end(), nc::IsGreaterThan<int>(13))));

	printf("Fill half array with zeroes\n");
	nc::fillN(array.begin(), Capacity/2, 0);
	printArray(array);

	printf("Fill the whole array with zeroes\n");
	nc::fill(array.begin(), array.end(), 0);
	printArray(array);

	nc::Array<int> array2(Capacity);

	printf("Filling a second array with random numbers\n");
	for (unsigned int i = 0; i < Capacity; i++)
	{
		array2[i] = rand() % 100;
	}
	printArray(array2);

	printf("Storing transformed random numbers back into the first array\n");
	nc::transform(array2.begin(), array2.end(), array.begin(), copyAddValue<10>);
	printArray(array);

	printf("Sorting the first array\n");
	nc::quicksort(array.begin(), array.end());
	printArray(array);
	printf("The array is %s\n", nc::isSorted(array.begin(), array.end()) ? "sorted" : "not sorted");

	printf("Reverse sorting the second array\n");
	nc::quicksortDesc(array2.begin(), array2.end());
	printArray(array2);
	printf("The array is %s\n", nc::isSorted(array2.begin(), array2.end(), nc::IsGreater<int>) ? "reverse sorted" : "not reverse sorted");

	printf("Reversing the first array\n");
	nc::reverse(array.begin(), array.end());
	printArray(array);

	printf("Reverse copying the first array into the second\n");
	nc::reverseCopy(array.begin(), array.end(), array2.begin());
	printArray(array2);

	printf("Remove all the elements greater than 50 from the second array\n");
	array2.erase(nc::removeIf(array2.begin(), array2.end(), nc::IsLessThan<int>(90)), array2.end());
	printArray(array2);

	return EXIT_SUCCESS;
}
