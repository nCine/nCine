#include "StaticArray.h"

namespace nc = ncine;

const unsigned int Size = 10;

void printArray(const nc::StaticArray<int, Size> &array)
{
	for (unsigned int i = 0; i < array.size(); i++)
	{
		printf("[%u]=%d ", i, array[i]);
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	nc::StaticArray<int, Size> array;

	printf("Initializing the whole array\n");
	for (unsigned int i = 0; i < Size; i++)
	{
		array[i] = i;
	}
	printArray(array);

//	printf("Writing beyond size\n");
//	array[Size] = Size;
//	printArray(array);


	printf("\n--- Array traversals (for cycles) ---\n");
	printf("Iterating through elements:");
	for (nc::StaticArray<int, Size>::Const_Iterator i = array.begin(); i != array.end(); ++i)
	{
		printf(" %d", *i);
	}
	printf("\n");

	printf("Iterating through elements (reverse):");
	for (nc::StaticArray<int, Size>::Const_Iterator r = array.rBegin(); r != array.rEnd(); --r)
	{
		printf(" %d", *r);
	}
	printf("\n");


	printf("\n--- Array traversals (while cycles) ---\n");
	printf("Iterating through elements:");
	nc::StaticArray<int, Size>::Const_Iterator i = array.begin();
	while (i != array.end())
	{
		printf(" %d", *i);
		++i;
	}
	printf("\n");

	printf("Iterating through elements (reverse):");
	nc::StaticArray<int, Size>::Const_Iterator r = array.rBegin();
	while (r != array.rEnd())
	{
		printf(" %d", *r);
		--r;
	}
	printf("\n");


	printf("\nCreating a second array with copy construction\n");
	nc::StaticArray<int, Size> array2(array);
	printArray(array2);

	printf("Creating a third array with the assignment operator\n");
	nc::StaticArray<int, Size> array3 = array;
	printArray(array3);

	return EXIT_SUCCESS;
}
