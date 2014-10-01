#include "ncServiceLocator.h"
#include "ncFileLogger.h"
#include "ncList.h"

void print_list(const ncList<int> &list)
{
	unsigned int uIndex = 0;

	for (ncList<int>::Const_Iterator i = list.Begin(); i != list.End(); ++i)
	{
		printf("[%u]=%d ", uIndex, *i);
		uIndex++;
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	ncServiceLocator::RegisterLogger(new ncFileLogger("log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));

	ncList<int> list;

	printf("Inserting some elements to the back\n");
	for (int i = 0; i < 11; i++)
	{
		list.InsertBack(i);
	}
	print_list(list);

	printf("Inserting one element at front\n");
	list.InsertFront(-1);
	print_list(list);

	printf("Removing first node\n");
	list.Remove(list.Begin());
	printf("Removing last node\n");
	list.Remove(list.RBegin());
	print_list(list);

	printf("Removing a couple of elements\n");
	list.Remove(3);
	list.Remove(5);
	print_list(list);

	printf("Removing a non-existant element\n");
	list.Remove(100);
	print_list(list);


	printf("\n--- List traversals (for cycles) ---\n");
	printf("Iterating through nodes:");
	for (ncList<int>::Iterator i = list.Begin(); i != list.End(); ++i)
	{
		printf(" %d", *i);
	}
	printf("\n");

	printf("Iterating through nodes (reverse):");
	for (ncList<int>::Iterator i = list.RBegin(); i != list.End(); --i)
	{
		printf(" %d", *i);
	}
	printf("\n");


	printf("\n--- List traversals (while cycles) ---\n");
	printf("Iterating through nodes:");
	ncList<int>::Iterator i = list.Begin();
	while (i != list.End())
	{
		printf(" %d", *i);
		++i;
	}
	printf("\n");

	printf("Iterating through nodes (reverse):");
	i = list.RBegin();
	while (i != list.End())
	{
		printf(" %d", *i);
		--i;
	}
	printf("\n");

	printf("\nClearing the whole list\n");
	if (!list.isEmpty())
	{
		list.Clear();
	}
	printf("Removing first node on empty\n");
	list.Remove(list.Begin());
	printf("Removing last node on empty\n");
	list.Remove(list.RBegin());
	print_list(list);

	return 0;
}
