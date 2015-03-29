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
	array.insertAt(3, 2);
	printArray(array);

	printf("Removing from the middle\n");
	array.removeAt(2);
	printArray(array);

	printf("Inserting as first and last\n");
	array.insertAt(0, -1);
	array.insertAt(array.size(), 12);
	printArray(array);

	printf("Removing the first and twice the last\n");
	array.removeAt(0);
	array.removeAt(array.size()-1);
	array.removeAt(array.size()-1);
	printArray(array);

	printf("Extending the size by two elements\n");
	unsigned int oldSize = array.size();
	array.setSize(oldSize + 2);
	// No "hole" when setting this element
	array[oldSize + 1] = 55;
	array[oldSize] = 44;
	printArray(array);
	printf("Restoring previous size\n");
	array.setSize(oldSize);
	printArray(array);
	unsigned int oldCapacity = array.capacity();
	printf("Current array capacity: %d, size:%d\n", array.capacity(), array.size());
	array.shrinkToFit();
	printf("New capacity after shrinkToFit(): %d, size: %d\n", array.capacity(), array.size());
	array.setCapacity(oldCapacity);
	printf("Restoring previous capacity: %d, size: %d\n", array.capacity(), array.size());

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
