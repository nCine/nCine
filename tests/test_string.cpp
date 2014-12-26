#include <cstdio>
#include "ncString.h"

namespace nc = ncine;

int main(int argc, char **argv)
{
	printf("Creating an empty string of a fixed size\n");
	const int Capacity = 16;
	nc::String str(Capacity);

	str = "String1";
	printf("Assigning a C string to it: \"%s\"\n", str.data());

	nc::String str2 = "String2";
	printf("Creating a second string from a C string assignment: \"%s\"\n", str2.data());

	nc::String str3(str);
	printf("Creating a third string as a copy of the first: \"%s\"\n", str3.data());

	if (str == str3)
	{
		printf("The first and the third string are equal\n");
	}

	if (str2 > str)
	{
		printf("The second string is lexicographically bigger than the first\n");
	}

	str += "Append";
	printf("Appending a C string to the first one: \"%s\"\n", str.data());

	str.clear();
	printf("Clearing the first string: \"%s\"\n", str.data());

	nc::String str4 = str3 + str2;
	printf("Creating a fourth string by appending the third and the second one: \"%s\"\n", str4.data());

	str.format("String%d", 1);
	printf("Resetting the first string to a formatted one: \"%s\"\n", str.data());

	str.formatAppend("String%d", 2);
	printf("Appending a formatted string to the first one: \"%s\"\n", str.data());

	printf("Printing the first character of the third string: %c\n", str3[0]);
	printf("Printing the last character of the third string: %c\n", str3[str3.length()-1]);
	printf("Accessing a character beyond the length of the third string: %c\n", str3[str3.length()]);

	printf("Printing the first position of the character 'S' in the fourth string: %d\n", str4.findFirstChar('S'));
	printf("Printing the last position of the character 'S' in the fourth string: %d\n", str4.findLastChar('S'));


	printf("\n--- String traversals (for cycles) ---\n");
	printf("Iterating through characters of first string:");
	for (nc::String::Const_Iterator i = str.begin(); i != str.end(); ++i)
	{
		printf(" %c", *i);
	}
	printf("\n");

	printf("Iterating through characters of first string (reverse):");
	for (nc::String::Const_Iterator r = str.revBegin(); r != str.revEnd(); --r)
	{
		printf(" %c", *r);
	}
	printf("\n");


	printf("\n--- String traversals (while cycles) ---\n");
	printf("Iterating through characters of first string:");
	nc::String::Const_Iterator i = str.begin();
	while (i != str.end())
	{
		printf(" %c", *i);
		++i;
	}
	printf("\n");

	printf("Iterating through characters of first string (reverse):");
	nc::String::Const_Iterator r = str.revBegin();
	while (r != str.revEnd())
	{
		printf(" %c", *r);
		--r;
	}
	printf("\n");

	return 0;
}
