#include "ncServiceLocator.h"
#include "ncFileLogger.h"
#include "base/ncList.h"

void print_list(const ncList<int> &list)
{
	unsigned int uIndex = 0;
	ncListNode<int> *pListNode = list.Head();

	while (pListNode)
	{
		printf("[%u]=%d ", uIndex, *pListNode);
		uIndex++;
		pListNode = pListNode->Next();
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	ncServiceLocator::RegisterLogger(new ncFileLogger("log.txt", 5, -1));

	ncList<int> list;

	printf("Inserting some elements to the back\n");
	for(int i = 0; i < 10; i++)
		list.InsertBack(i);
	print_list(list);

	printf("Inserting at front\n");
	list.InsertFront(-1);
	print_list(list);

	printf("Removing a couple of elements\n");
	list.Remove(3);
	list.Remove(5);
	print_list(list);

	printf("Removing a non-existant element\n");
	list.Remove(100);
	print_list(list);

	printf("Clearing the whole list\n");
	if (!list.isEmpty())
		list.Clear();
	print_list(list);

	return 0;	
}
