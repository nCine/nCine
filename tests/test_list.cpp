#include "List.h"

namespace nc = ncine;

void printList(const nc::List<int> &list)
{
	unsigned int index = 0;

	for (nc::List<int>::Const_Iterator i = list.begin(); i != list.end(); ++i)
	{
		printf("[%u]=%d ", index, *i);
		index++;
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	nc::List<int> list;

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
	for (nc::List<int>::Const_Iterator i = list.begin(); i != list.end(); ++i)
	{
		printf(" %d", *i);
	}
	printf("\n");

	printf("Iterating through nodes (reverse):");
	for (nc::List<int>::Const_Iterator r = list.revBegin(); r != list.end(); --r)
	{
		printf(" %d", *r);
	}
	printf("\n");


	printf("\n--- List traversals (while cycles) ---\n");
	printf("Iterating through nodes:");
	nc::List<int>::Const_Iterator i = list.begin();
	while (i != list.end())
	{
		printf(" %d", *i);
		++i;
	}
	printf("\n");

	printf("Iterating through nodes (reverse):");
	nc::List<int>::Const_Iterator r = list.revBegin();
	while (r != list.end())
	{
		printf(" %d", *r);
		--r;
	}
	printf("\n");


	printf("\nCreating a second list with copy construction\n");
	nc::List<int> list2(list);

	printf("Creating a third list with the assignment operator\n");
	nc::List<int> list3 = list;

	printf("Clearing the first list\n");
	if (!list.isEmpty())
	{
		list.clear();
	}
	printf("Removing first node on empty\n");
	list.remove(list.begin());
	printf("Removing last node on empty\n");
	list.remove(list.revBegin());
	printList(list);

	printf("Contents of second list\n");
	printList(list2);

	printf("Contents of third list\n");
	printList(list3);

	return 0;
}
