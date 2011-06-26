#include "ncServiceLocator.h"
#include "ncFileLogger.h"
#include "base/ncArray.h"

void print_array(const ncArray<int> &array)
{
	for (unsigned int i = 0; i < array.Size(); i++)
		printf("[%u]=%d ", i, array[i]);
	printf("\n");
}

int main(int argc, char **argv)
{
	ncServiceLocator::RegisterLogger(new ncFileLogger("log.txt", 5, -1));

	const int iCap = 10;
	ncArray<int> array(iCap);

//	printf("Trying to create a hole\n");
//	array[1] = 1;

	printf("Filling the whole array\n");
	for(int i = 0; i < iCap; i++)
		array[i] = i;
	print_array(array);

	printf("Writing beyond capacity\n");
	array[iCap] = iCap;
	print_array(array);

	printf("Inserting at the back\n");
	array.InsertBack(iCap+1);
	print_array(array);

	printf("Overwriting at the middle\n");
	array[2] = 22;
	print_array(array);

	printf("Inserting in the middle\n");
	array.InsertAt(3, 33);
	print_array(array);

	printf("Removing twice from the middle\n");
	array.RemoveAt(2);
	array.RemoveAt(2);
	print_array(array);

	printf("Clearing the whole array\n");
	if (!array.isEmpty())
		array.Clear();
	print_array(array);

	return 0;	
}
