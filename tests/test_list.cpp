#include "ncServiceLocator.h"
#include "ncFileLogger.h"
#include "ncList.h"

void printList(const ncList<int> &list)
{
	unsigned int index = 0;

	for (ncList<int>::Const_Iterator i = list.begin(); i != list.end(); ++i)
	{
		printf("[%u]=%d ", index, *i);
		index++;
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	ncServiceLocator::registerLogger(new ncFileLogger("log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));

	ncList<int> list;

	printf("Inserting some elements to the back\n");
	for (int i = 0; i < 11; i++)
	{
		list.insertBack(i);
	}
	printList(list);

	printf("Inserting one element at front\n");
	list.insertFront(-1);
	printList(list);

	printf("Removing first node\n");
	list.remove(list.begin());
	printf("Removing last node\n");
	list.remove(list.revBegin());
	printList(list);

	printf("Removing a couple of elements\n");
	list.remove(3);
	list.remove(5);
	printList(list);

	printf("Removing a non-existant element\n");
	list.remove(100);
	printList(list);


	printf("\n--- List traversals (for cycles) ---\n");
	printf("Iterating through nodes:");
	for (ncList<int>::Iterator i = list.begin(); i != list.end(); ++i)
	{
		printf(" %d", *i);
	}
	printf("\n");

	printf("Iterating through nodes (reverse):");
	for (ncList<int>::Iterator i = list.revBegin(); i != list.end(); --i)
	{
		printf(" %d", *i);
	}
	printf("\n");


	printf("\n--- List traversals (while cycles) ---\n");
	printf("Iterating through nodes:");
	ncList<int>::Iterator i = list.begin();
	while (i != list.end())
	{
		printf(" %d", *i);
		++i;
	}
	printf("\n");

	printf("Iterating through nodes (reverse):");
	i = list.revBegin();
	while (i != list.end())
	{
		printf(" %d", *i);
		--i;
	}
	printf("\n");

	printf("\nClearing the whole list\n");
	if (!list.isEmpty())
	{
		list.clear();
	}
	printf("Removing first node on empty\n");
	list.remove(list.begin());
	printf("Removing last node on empty\n");
	list.remove(list.revBegin());
	printList(list);

	return 0;
}
