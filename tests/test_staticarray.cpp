#include "StaticArray.h"

namespace nc = ncine;

namespace {

const unsigned int Capacity = 10;

void printArray(const nc::StaticArray<int, Capacity> &array)
{
	for (unsigned int i = 0; i < array.size(); i++)
		printf("[%u]=%d ", i, array[i]);
	printf("\n");
}

}

int main(int argc, char **argv)
{
	nc::StaticArray<int, Capacity> array;

	printf("Initializing the whole array\n");
	for (unsigned int i = 0; i < Capacity; i++)
		array[i] = i;
	printArray(array);

//	printf("Writing beyond size\n");
//	array[Size] = Size;
//	printArray(array);


	printf("\n--- Array traversals (for cycles) ---\n");
	printf("Iterating through elements:");
	for (nc::StaticArray<int, Capacity>::ConstIterator i = array.begin(); i != array.end(); ++i)
		printf(" %d", *i);
	printf("\n");

	printf("Iterating through elements (reverse):");
	for (nc::StaticArray<int, Capacity>::ConstIterator r = array.rBegin(); r != array.rEnd(); --r)
		printf(" %d", *r);
	printf("\n");


	printf("\n--- Array traversals (while cycles) ---\n");
	printf("Iterating through elements:");
	nc::StaticArray<int, Capacity>::ConstIterator i = array.begin();
	while (i != array.end())
	{
		printf(" %d", *i);
		++i;
	}
	printf("\n");

	printf("Iterating through elements (reverse):");
	nc::StaticArray<int, Capacity>::ConstIterator r = array.rBegin();
	while (r != array.rEnd())
	{
		printf(" %d", *r);
		--r;
	}
	printf("\n");


	printf("\nCreating a second array with copy construction\n");
	nc::StaticArray<int, Capacity> array2(array);
	printArray(array2);

	printf("Creating a third array with the assignment operator\n");
	nc::StaticArray<int, Capacity> array3 = array;
	printArray(array3);

	printf("Clearing the first array\n");
	if (!array.isEmpty())
		array.clear();
	printArray(array);

	printf("Inserting twice at the back\n");
	array.insertBack(1);
	array.insertBack(2);
	printArray(array);

	return EXIT_SUCCESS;
}
