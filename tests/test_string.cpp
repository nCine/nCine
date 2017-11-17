#include <cstdlib> // for EXIT_SUCCESS
#include <cstdio>
#include "ncString.h"

namespace nc = ncine;

int main(int argc, char **argv)
{
	const int Capacity = 16;
	nc::String str(Capacity);
	printf("Creating an empty string: \"%s\" (capacity %u, length %u)\n", str.data(), str.capacity(), str.length());

	str = "String1";
	printf("Assigning a C-style string to it: \"%s\" (capacity %u, length %u)\n", str.data(), str.capacity(), str.length());

	nc::String str2 = "String2";
	printf("Creating a second string from a C-style string assignment: \"%s\" (capacity %u, length %u)\n", str2.data(), str2.capacity(), str2.length());

	nc::String str3(str);
	printf("Creating a third string as a copy of the first one: \"%s\" (capacity %u, length %u)\n", str3.data(), str3.capacity(), str3.length());

	printf("The first and the third string are equal: %d\n", str == str3);
	printf("The first and the second string are not equal: %d\n", str != str2);
	printf("The second string is lexicographically bigger than the first: %d\n", str2 > str);

	str += "Append";
	printf("Appending a C-style string to the first one: \"%s\" (capacity %u, length %u)\n", str.data(), str.capacity(), str.length());

	str.clear();
	printf("Clearing the first string: \"%s\" (capacity %u, length %u)\n", str.data(), str.capacity(), str.length());

	nc::String str4 = str3 + str2;
	printf("Creating a fourth string by appending the third and the second one: \"%s\" (capacity %u, length %u)\n", str4.data(), str4.capacity(), str4.length());

	str.format("String%d", 1);
	printf("Resetting the first string to a formatted one: \"%s\", (capacity %u, length %u)\n", str.data(), str.capacity(), str.length());

	str.formatAppend("String%d", 2);
	printf("Appending a formatted string to the first one: \"%s\" (capacity %u, length %u)\n", str.data(), str.capacity(), str.length());

	printf("Printing the first character of the third string: %c\n", str3[0]);
	printf("Printing the last character of the third string: %c\n", str3[str3.length() - 1]);
	printf("Accessing a character beyond the length of the third string: %c\n", str3[str3.length()]);

	printf("Printing the first position of the character 'S' in the fourth string: %d\n", str4.findFirstChar('S'));
	printf("Printing the last position of the character 'S' in the fourth string: %d\n", str4.findLastChar('S'));
	printf("Printing the position of the character 't' after the second in the fourth string: %d\n", str4.findFirstCharAfterIndex('t', 1));


	str2.copy(str3, 1, 2, 3);
	printf("Copying some characters from the third string into the second: \"%s\" (capacity %u, length %u)\n", str2.data(), str2.capacity(), str2.length());
	str2.copy(str3, 3, 2, 3);
	printf("Copied some characters from the third string into the second: \"%s\" (capacity %u, length %u)\n", str2.data(), str2.capacity(), str2.length());
	str2.copy(str3, str3.length() + 1, 1, 0);
	printf("Trying to copy from beyond the end of the third string into the second: \"%s\" (capacity %u, length %u)\n", str2.data(), str2.capacity(), str2.length());
	str2.copy(str3, 0, 1, str2.length() + 1);
	printf("Trying to copy from the third string to beyond the end of the second: \"%s\" (capacity %u, length %u)\n", str2.data(), str2.capacity(), str2.length());
	str2.copy(str3, 0, str3.length() + 1, 0);
	printf("Trying to copy from the third string to the second more than the available characters: \"%s\" (capacity %u, length %u)\n", str2.data(), str2.capacity(), str2.length());

	const unsigned int CStringLength = 4;
	char cString[CStringLength];
	str3.copyTo(cString, 2, CStringLength);
	printf("Copying some characters from the third string into an array of %u characters: \"%s\"\n", CStringLength, cString);


	printf("\n--- String traversals (for cycles) ---\n");
	printf("Iterating through characters of first string:");
	for (nc::String::ConstIterator i = str.begin(); i != str.end(); ++i)
		printf(" %c", *i);
	printf("\n");

	printf("Iterating through characters of first string (reverse):");
	for (nc::String::ConstIterator r = str.rBegin(); r != str.rEnd(); --r)
		printf(" %c", *r);
	printf("\n");


	printf("\n--- String traversals (while cycles) ---\n");
	printf("Iterating through characters of first string:");
	nc::String::ConstIterator i = str.begin();
	while (i != str.end())
	{
		printf(" %c", *i);
		++i;
	}
	printf("\n");

	printf("Iterating through characters of first string (reverse):");
	nc::String::ConstIterator r = str.rBegin();
	while (r != str.rEnd())
	{
		printf(" %c", *r);
		--r;
	}
	printf("\n");

	return EXIT_SUCCESS;
}
