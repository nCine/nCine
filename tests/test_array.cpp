#include "Array.h"

namespace nc = ncine;

void printArray(const nc::Array<int> &array)
{
	for (unsigned int i = 0; i < array.size(); i++)
	{
		printf("[%u]=%d ", i, array[i]);
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	const unsigned int Capacity = 10;
	nc::Array<int> array(Capacity);

//	printf("Trying to create a hole\n");
//	array[1] = 1;

	printf("Filling the whole array\n");
	for (unsigned int i = 0; i < Capacity; i++)
	{
		array[i] = i;
	}
	printArray(array);

	printf("Writing beyond capacity\n");
	array[Capacity] = Capacity;
	printArray(array);

	printf("Inserting at the back\n");
	array.insertBack(Capacity + 1);
	printArray(array);

	printf("Overwriting at the middle\n");
	array[2] = 22;
	printArray(array);

	printf("Inserting in the middle\n");
	array.insertAt(3, 33);
	printArray(array);

	printf("Removing twice from the middle\n");
	array.removeAt(2);
	array.removeAt(2);
	printArray(array);


	printf("\n--- Array traversals (for cycles) ---\n");
	printf("Iterating through elements:");
	for (nc::Array<int>::Const_Iterator i = array.begin(); i != array.end(); ++i)
	{
		printf(" %d", *i);
	}
	printf("\n");

	printf("Iterating through elements (reverse):");
	for (nc::Array<int>::Const_Iterator r = array.rBegin(); r != array.rEnd(); --r)
	{
		printf(" %d", *r);
	}
	printf("\n");


	printf("\n--- Array traversals (while cycles) ---\n");
	printf("Iterating through elements:");
	nc::Array<int>::Const_Iterator i = array.begin();
	while (i != array.end())
	{
		printf(" %d", *i);
		++i;
	}
	printf("\n");

	printf("Iterating through elements (reverse):");
	nc::Array<int>::Const_Iterator r = array.rBegin();
	while (r != array.rEnd())
	{
		printf(" %d", *r);
		--r;
	}
	printf("\n");


	printf("\nCreating a second array with copy construction\n");
	nc::Array<int> array2(array);

	printf("Creating a third array with the assignment operator\n");
	nc::Array<int> array3 = array;

	printf("Clearing the first array\n");
	if (!array.isEmpty())
	{
		array.clear();
	}
	printArray(array);

	printf("Contents of second array\n");
	printArray(array2);

	printf("Contents of third array\n");
	printArray(array3);

	return EXIT_SUCCESS;
}
