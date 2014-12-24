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
	const int Capacity = 10;
	nc::Array<int> array(Capacity);

//	printf("Trying to create a hole\n");
//	array[1] = 1;

	printf("Filling the whole array\n");
	for (int i = 0; i < Capacity; i++)
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

	return 0;
}
