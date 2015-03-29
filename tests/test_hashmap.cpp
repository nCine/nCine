#include <cstdlib> // for EXIT_SUCCESS
#include "HashMap.h"
#include "HashMapIterator.h"

namespace nc = ncine;

int main(int argc, char **argv)
{
	printf("Creating a hashmap of numbers\n");
	//nc::HashMap<int, int, nc::IdentityHashFunc<int> > hashmap(32);
	nc::HashMap<int, int, nc::FixedHashFunc<int> > hashmap(32);

	printf("Number of buckets: %d\n", hashmap.bucketAmount());
	printf("Inserting some elements\n");
	for (int i = 0; i < 10; i++)
	{
		hashmap[i] = i + 10;
	}

	printf("Retrieving the elements:\n");
	for (int i = 0; i < 10; i++)
	{
		printf("key: %d, value: %d\n", i, hashmap[i]);
	}

	printf("Removing a couple elements\n");
	hashmap.remove(5);
	hashmap.remove(7);

	printf("Creating a second hashmap as a copy of the first\n");
	nc::HashMap<int, int, nc::FixedHashFunc<int> > hashmap2(hashmap);
	printf("Creating a third hashmap and then assign the first to it\n");
	nc::HashMap<int, int, nc::FixedHashFunc<int> > hashmap3(32);
	hashmap3 = hashmap;

	int value = 0;
	bool found = hashmap.contains(1, value);
	printf("Key 1 is in the hashmap: %d - Value: %d\n", found, value);
	value = 0;
	found = hashmap.contains(10, value);
	printf("Key 10 is in the hashmap: %d - Value: %d\n", found, value);

	printf("Bucket size for key 0: %d\n", hashmap.bucketSize(0));


	printf("\n--- HashMap traversals (for cycles) ---\n");
	printf("Iterating through the elements of second hashmap (copy constructed):\n");
	for (const nc::HashMapIterator<int, int, nc::FixedHashFunc<int> > i = i.begin(hashmap2); i != i.end(hashmap2); ++i)
	{
		printf("hash: %lu, key: %d, value: %d\n", (*i).hash, (*i).key, (*i).value);
	}

	printf("\n--- HashMap traversals (while cycles) ---\n");
	printf("Iterating through the elements of the third hashmap (created and then assigned):\n");
	const nc::HashMapIterator<int, int, nc::FixedHashFunc<int> > i1 = i1.begin(hashmap3);
	while (i1 != i1.end(hashmap3))
	{
		printf("hash: %lu, key: %d, value: %d\n", (*i1).hash, (*i1).key, (*i1).value);
		++i1;
	}

	return EXIT_SUCCESS;
}
