#include <cstdlib> // for EXIT_SUCCESS
#include "HashMap.h"
#include "HashMapIterator.h"
#include "ncString.h"

namespace nc = ncine;

int main(int argc, char **argv)
{
	printf("Creating a hashmap of strings\n");
	nc::StringHashMap<nc::String>::Type strHashmap(32);

	printf("Number of buckets: %d\n", strHashmap.bucketAmount());
	printf("Inserting some elements\n");
	strHashmap["A"] = "AAAA";
	strHashmap["a"] = "aaaa";
	strHashmap["B"] = "BBBB";
	strHashmap["C"] = "CCCC";
	strHashmap["AB"] = "ABABABAB";
	strHashmap["BA"] = "BABABABA";

	printf("Retrieving the elements:\n");
	printf("key: %s, value: %s\n", "A", strHashmap["A"].data());
	printf("key: %s, value: %s\n", "a", strHashmap["a"].data());
	printf("key: %s, value: %s\n", "B", strHashmap["B"].data());
	printf("key: %s, value: %s\n", "C", strHashmap["C"].data());
	printf("key: %s, value: %s\n", "AB", strHashmap["AB"].data());
	printf("key: %s, value: %s\n", "BA", strHashmap["BA"].data());

	printf("Removing a couple elements\n");
	strHashmap.remove("a");
	strHashmap.remove("C");

	printf("Creating a second hashmap as a copy of the first\n");
	nc::StringHashMap<nc::String>::Type strHashmap2(strHashmap);
	printf("Creating a third hashmap and then assign the first to it\n");
	nc::StringHashMap<nc::String>::Type strHashmap3(32);
	strHashmap3 = strHashmap;

	nc::String value;
	bool found = strHashmap.contains("A", value);
	printf("Key A is in the hashmap: %d - Value: %s\n", found, value.data());
	value.clear();
	found = strHashmap.contains("C", value);
	printf("Key C is in the hashmap: %d - Value: %s\n", found, value.data());

	printf("\n--- HashMap traversals (for cycles) ---\n");
	printf("Iterating through the elements of the second hashmap (copy constructed)\n");
	for (nc::StringHashMap<nc::String>::ConstIterator i = strHashmap2.begin(); i != strHashmap2.end(); ++i)
		printf("hash: %lu, key: %s, value: %s\n", i.hash(), i.key().data(), i.value().data());

	printf("\n--- HashMap traversals (while cycles) ---\n");
	printf("Iterating through the elements of the third hashmap (created and then assigned)\n");
	nc::StringHashMap<nc::String>::ConstIterator i2 = strHashmap3.begin();
	while (i2 != strHashmap3.end())
	{
		printf("hash: %lu, key: %s, value: %s\n", i2.hash(), i2.key().data(), i2.value().data());
		++i2;
	}

	return EXIT_SUCCESS;
}
