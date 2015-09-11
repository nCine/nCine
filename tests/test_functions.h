#ifndef TEST_FUNCTIONS
#define TEST_FUNCTIONS

#include "Array.h"
#include "List.h"
#include "Matrix4x4.h"

namespace nc = ncine;

namespace {

inline void printArray(const nc::Array<int> &array)
{
	printf("Size %d: ", array.size());
	for (unsigned int i = 0; i < array.size(); i++)
	{
		printf("[%u]=%d ", i, array[i]);
	}
	printf("\n");
}

inline void printList(const nc::List<int> &list)
{
	unsigned int index = 0;

	printf("Size %d: ", list.size());
	for (nc::List<int>::ConstIterator i = list.begin(); i != list.end(); ++i)
	{
		printf("[%u]=%d ", index, *i);
		index++;
	}
	printf("\n");
}

inline void printMatrix4x4f(const nc::Matrix4x4f& mat)
{
	printf("[");
	for (unsigned int i = 0; i < 4; i++)
	{
		if (i > 0)
		{
			printf(" ");
		}
		printf("%.2f,\t%.2f,\t%.2f,\t%.2f", mat[i].x, mat[i].y, mat[i].z, mat[i].w);
		if (i < 3)
		{
			printf(",\n");
		}
	}
	printf("]\n");
}

template<int value>
class AddValue
{
  public:
	inline void operator()(int& n) const { n += value; }
};

template<int value>
inline void addValue(int& n) { n += value; }

template<int value>
inline int copyAddValue(const int& n) { return n + value; }

}

#endif
