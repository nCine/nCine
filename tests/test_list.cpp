#include <cstdlib> // for EXIT_SUCCESS
#include "test_functions.h"

namespace {

bool isOdd(int n) { return (n % 2 != 0); }
bool isEven(int n) { return (n % 2 == 0); }

}

int main(int argc, char **argv)
{
	nc::List<int> list;

	printf("Inserting some elements to the back\n");
	for (int i = 0; i < 11; i++)
		list.pushBack(i);
	printList(list);

	printf("Inserting one element at the front and one at the back\n");
	list.pushFront(-1);
	list.pushBack(-1);
	printList(list);

	printf("Removing first and last node\n");
	list.popFront();
	list.popBack();
	printList(list);

	printf("Removing first and last node with iterators\n");
	list.erase(list.begin());
	list.erase(list.rBegin());
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
	for (nc::List<int>::ConstIterator i = list.begin(); i != list.end(); ++i)
		printf(" %d", *i);
	printf("\n");

	printf("Iterating through nodes (reverse):");
	for (nc::List<int>::ConstIterator r = list.rBegin(); r != list.rEnd(); --r)
		printf(" %d", *r);
	printf("\n");


	printf("\n--- List traversals (while cycles) ---\n");
	printf("Iterating through nodes:");
	nc::List<int>::ConstIterator i = list.begin();
	while (i != list.end())
	{
		printf(" %d", *i);
		++i;
	}
	printf("\n");

	printf("Iterating through nodes (reverse):");
	nc::List<int>::ConstIterator r = list.rBegin();
	while (r != list.rEnd())
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
		list.clear();
	printf("Removing first node on empty\n");
	list.erase(list.begin());
	printf("Removing last node on empty\n");
	list.erase(list.rBegin());
	printList(list);

	printf("Contents of second list\n");
	printList(list2);

	printf("Contents of third list\n");
	printList(list3);

	printf("Removing all odd numbers from the second list\n");
	list2.removeIf(isOdd);
	printList(list2);

	printf("Removing all even numbers from the third list\n");
	list3.removeIf(isEven);
	printList(list3);

	printf("Removing the first and the last element of the second list through iterators\n");
	list2.erase(list2.begin());
	list2.erase(list2.end());
	printList(list2);

	printf("Removing all the elements of the third list through iterators\n");
	list3.erase(list3.begin(), list3.end());
	printList(list3);

	printf("Inserting all elements from the second list to the third through iterators\n");
	list3.insert(list3.begin(), list2.begin(), list2.end());
	printList(list3);
	printf("\n");


	printf("Recreating the first list\n");
	list.clear();
	for (int j = 0; j < 11; j++)
		list.pushBack(j);
	printList(list);
	printf("\n");

	printf("Splicing the sixth element of the first list into the second\n");
	list2.splice(list2.begin(), list, nc::next(list.begin(), 5));
	printf("List 1 - "); printList(list);
	printf("List 2 - "); printList(list2);
	printf("\n");

	printf("Splicing the seventh element of the first list into the second\n");
	list2.splice(list2.begin(), list, nc::next(list.begin(), 6));
	printf("List 1 - "); printList(list);
	printf("List 2 - "); printList(list2);
	printf("\n");

	printf("Splicing the fourth element of the second list into the third\n");
	list3.splice(list3.begin(), list2, nc::next(list2.begin(), 3));
	printf("List 2 - "); printList(list2);
	printf("List 3 - "); printList(list3);
	printf("\n");

	printf("Splicing the whole second list at the end of the third\n");
	list3.splice(list3.rBegin(), list2);
	printf("List 2 - "); printList(list2);
	printf("List 3 - "); printList(list3);
	printf("\n");


	printf("Splicing a range from the first list to the third\n");
	list3.splice(nc::next(list3.begin(), 2), list, nc::next(list.begin(), 2), nc::next(list.rBegin(), -3));
	printf("List 1 - ");
	printList(list);
	printf("List 3 - ");
	printList(list3);

	printf("Splicing a range from the third list to the second\n");
	list2.splice(list2.begin(), list3, nc::next(list3.begin(), 5), list3.end());
	printf("List 2 - ");
	printList(list2);
	printf("List 3 - ");
	printList(list3);

	return EXIT_SUCCESS;
}
