#include "gtest_allocators.h"
#include <nctl/Array.h>
#include <nctl/List.h>
#include <nctl/HashMap.h>
#include <nctl/HashMapList.h>
#include <nctl/HashSet.h>
#include <nctl/HashSetList.h>
#include <nctl/SparseSet.h>

namespace {

using HashMapType = nctl::HashMap<unsigned int, ElementType, nctl::FNV1aHashFunc<unsigned int>>;
using HashMapListType = nctl::HashMapList<unsigned int, ElementType, nctl::FNV1aHashFunc<unsigned int>>;
using HashSetType = nctl::HashSet<ElementType, ElementTypeStructHashFunc>;
using HashSetListType = nctl::HashSetList<ElementType, ElementTypeStructHashFunc>;

class AllocatorContainersTest : public ::testing::Test
{
  protected:
	static const size_t BufferSize_ = Capacity * ElementSize + 16384;
	uint8_t buffer1_[BufferSize_];
	uint8_t buffer2_[BufferSize_];
};

TEST_F(AllocatorContainersTest, CopyConstructArray)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);

	printf("Creating an array with a custom allocator\n");
	nctl::Array<ElementType> array1(NumElements, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		array1.emplaceBack(i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second array with copy construction\n");
	nctl::Array<ElementType> array2(array1);
	ASSERT_EQ(array2.size(), NumElements);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		ASSERT_EQ(array2[i].a, i);
		ASSERT_EQ(array2[i].b, i + 1);
		ASSERT_EQ(array2[i].c, i + 2.0f);
		ASSERT_EQ(array2[i].d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, MoveConstructArray)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);

	printf("Creating an array with a custom allocator\n");
	nctl::Array<ElementType> array1(NumElements, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		array1.emplaceBack(i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second array with move construction\n");
	nctl::Array<ElementType> array2(nctl::move(array1));
	ASSERT_EQ(array2.size(), NumElements);
	ASSERT_EQ(array1.size(), 0);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		ASSERT_EQ(array2[i].a, i);
		ASSERT_EQ(array2[i].b, i + 1);
		ASSERT_EQ(array2[i].c, i + 2.0f);
		ASSERT_EQ(array2[i].d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, CopyAssignArray)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);
	nctl::FreeListAllocator freelistAllocator2(BufferSize_, &buffer2_);

	printf("Creating an array with a custom allocator\n");
	nctl::Array<ElementType> array1(NumElements, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		array1.emplaceBack(i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second array with a different custom allocator\n");
	nctl::Array<ElementType> array2(NumElements, freelistAllocator2);
	printf("Copy assign the first array to the second\n");
	array2 = array1;
	ASSERT_EQ(array2.size(), NumElements);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		ASSERT_EQ(array2[i].a, i);
		ASSERT_EQ(array2[i].b, i + 1);
		ASSERT_EQ(array2[i].c, i + 2.0f);
		ASSERT_EQ(array2[i].d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, MoveAssignArray)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);
	nctl::FreeListAllocator freelistAllocator2(BufferSize_, &buffer2_);

	printf("Creating an array with a custom allocator\n");
	nctl::Array<ElementType> array1(NumElements, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		array1.emplaceBack(i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second array with a different custom allocator\n");
	nctl::Array<ElementType> array2(NumElements, freelistAllocator2);
	printf("Move assign the first array to the second\n");
	array2 = nctl::move(array1);
	ASSERT_EQ(array2.size(), NumElements);
	ASSERT_EQ(array1.size(), 0);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		ASSERT_EQ(array2[i].a, i);
		ASSERT_EQ(array2[i].b, i + 1);
		ASSERT_EQ(array2[i].c, i + 2.0f);
		ASSERT_EQ(array2[i].d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, CopyConstructList)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);

	printf("Creating a list with a custom allocator\n");
	nctl::List<ElementType> list1(freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		list1.emplaceBack(i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second list with copy construction\n");
	nctl::List<ElementType> list2(list1);
	ASSERT_EQ(list2.size(), NumElements);
	unsigned int count = 0;
	for (nctl::List<ElementType>::ConstIterator i = list2.begin(); i != list2.end(); ++i)
	{
		ASSERT_EQ((*i).a, count);
		ASSERT_EQ((*i).b, count + 1);
		ASSERT_EQ((*i).c, count + 2.0f);
		ASSERT_EQ((*i).d, count + 3.0f);
		count++;
	}
}

TEST_F(AllocatorContainersTest, MoveConstructList)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);

	printf("Creating a list with a custom allocator\n");
	nctl::List<ElementType> list1(freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		list1.emplaceBack(i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second list with move construction\n");
	nctl::List<ElementType> list2(nctl::move(list1));
	ASSERT_EQ(list2.size(), NumElements);
	ASSERT_EQ(list1.size(), 0);
	unsigned int count = 0;
	for (nctl::List<ElementType>::ConstIterator i = list2.begin(); i != list2.end(); ++i)
	{
		ASSERT_EQ((*i).a, count);
		ASSERT_EQ((*i).b, count + 1);
		ASSERT_EQ((*i).c, count + 2.0f);
		ASSERT_EQ((*i).d, count + 3.0f);
		count++;
	}
}

TEST_F(AllocatorContainersTest, CopyAssignList)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);
	nctl::FreeListAllocator freelistAllocator2(BufferSize_, &buffer2_);

	printf("Creating a list with a custom allocator\n");
	nctl::List<ElementType> list1(freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		list1.emplaceBack(i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second list with a different custom allocator\n");
	nctl::List<ElementType> list2(freelistAllocator2);
	printf("Copy assign the first list to the second\n");
	list2 = list1;
	ASSERT_EQ(list2.size(), NumElements);
	unsigned int count = 0;
	for (nctl::List<ElementType>::ConstIterator i = list2.begin(); i != list2.end(); ++i)
	{
		ASSERT_EQ((*i).a, count);
		ASSERT_EQ((*i).b, count + 1);
		ASSERT_EQ((*i).c, count + 2.0f);
		ASSERT_EQ((*i).d, count + 3.0f);
		count++;
	}
}

TEST_F(AllocatorContainersTest, MoveAssignList)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);
	nctl::FreeListAllocator freelistAllocator2(BufferSize_, &buffer2_);

	printf("Creating a list with a custom allocator\n");
	nctl::List<ElementType> list1(freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		list1.emplaceBack(i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second list with a different custom allocator\n");
	nctl::List<ElementType> list2(freelistAllocator2);
	printf("Move assign the first list to the second\n");
	list2 = nctl::move(list1);
	ASSERT_EQ(list2.size(), NumElements);
	ASSERT_EQ(list1.size(), 0);
	unsigned int count = 0;
	for (nctl::List<ElementType>::ConstIterator i = list2.begin(); i != list2.end(); ++i)
	{
		ASSERT_EQ((*i).a, count);
		ASSERT_EQ((*i).b, count + 1);
		ASSERT_EQ((*i).c, count + 2.0f);
		ASSERT_EQ((*i).d, count + 3.0f);
		count++;
	}
}

TEST_F(AllocatorContainersTest, CopyConstructHashMap)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);

	printf("Creating a hashmap with a custom allocator\n");
	HashMapType hashmap1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashmap1.emplace(i, i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second hashmap with copy construction\n");
	HashMapType hashmap2(hashmap1);
	ASSERT_EQ(hashmap2.size(), NumElements);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		ASSERT_EQ(hashmap2[i].a, i);
		ASSERT_EQ(hashmap2[i].b, i + 1);
		ASSERT_EQ(hashmap2[i].c, i + 2.0f);
		ASSERT_EQ(hashmap2[i].d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, MoveConstructHashMap)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);

	printf("Creating a hashmap with a custom allocator\n");
	HashMapType hashmap1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashmap1.emplace(i, i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second hashmap with move construction\n");
	HashMapType hashmap2(nctl::move(hashmap1));
	ASSERT_EQ(hashmap2.size(), NumElements);
	ASSERT_EQ(hashmap1.size(), 0);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		ASSERT_EQ(hashmap2[i].a, i);
		ASSERT_EQ(hashmap2[i].b, i + 1);
		ASSERT_EQ(hashmap2[i].c, i + 2.0f);
		ASSERT_EQ(hashmap2[i].d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, CopyAssignHashMap)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);
	nctl::FreeListAllocator freelistAllocator2(BufferSize_, &buffer2_);

	printf("Creating a hashmap with a custom allocator\n");
	HashMapType hashmap1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashmap1.emplace(i, i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second hashmap with a different custom allocator\n");
	HashMapType hashmap2(Capacity, freelistAllocator2);
	printf("Copy assign the first hashmap to the second\n");
	hashmap2 = hashmap1;
	ASSERT_EQ(hashmap2.size(), NumElements);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		ASSERT_EQ(hashmap2[i].a, i);
		ASSERT_EQ(hashmap2[i].b, i + 1);
		ASSERT_EQ(hashmap2[i].c, i + 2.0f);
		ASSERT_EQ(hashmap2[i].d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, MoveAssignHashMap)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);
	nctl::FreeListAllocator freelistAllocator2(BufferSize_, &buffer2_);

	printf("Creating a hashmap with a custom allocator\n");
	HashMapType hashmap1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashmap1.emplace(i, i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second hashmap with a different custom allocator\n");
	HashMapType hashmap2(Capacity, freelistAllocator2);
	printf("Move assign the first hashmap to the second\n");
	hashmap2 = nctl::move(hashmap1);
	ASSERT_EQ(hashmap2.size(), NumElements);
	ASSERT_EQ(hashmap1.size(), 0);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		ASSERT_EQ(hashmap2[i].a, i);
		ASSERT_EQ(hashmap2[i].b, i + 1);
		ASSERT_EQ(hashmap2[i].c, i + 2.0f);
		ASSERT_EQ(hashmap2[i].d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, CopyConstructHashMapList)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);

	printf("Creating a hashmap with a custom allocator\n");
	HashMapListType hashmap1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashmap1.emplace(i, i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second hashmap with copy construction\n");
	HashMapListType hashmap2(hashmap1);
	ASSERT_EQ(hashmap2.size(), NumElements);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		ASSERT_EQ(hashmap2[i].a, i);
		ASSERT_EQ(hashmap2[i].b, i + 1);
		ASSERT_EQ(hashmap2[i].c, i + 2.0f);
		ASSERT_EQ(hashmap2[i].d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, MoveConstructHashMapList)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);

	printf("Creating a hashmap with a custom allocator\n");
	HashMapListType hashmap1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashmap1.emplace(i, i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second hashmap with move construction\n");
	HashMapListType hashmap2(nctl::move(hashmap1));
	ASSERT_EQ(hashmap2.size(), NumElements);
	ASSERT_EQ(hashmap1.size(), 0);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		ASSERT_EQ(hashmap2[i].a, i);
		ASSERT_EQ(hashmap2[i].b, i + 1);
		ASSERT_EQ(hashmap2[i].c, i + 2.0f);
		ASSERT_EQ(hashmap2[i].d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, CopyAssignHashMapList)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);
	nctl::FreeListAllocator freelistAllocator2(BufferSize_, &buffer2_);

	printf("Creating a hashmap with a custom allocator\n");
	HashMapListType hashmap1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashmap1.emplace(i, i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second hashmap with a different custom allocator\n");
	HashMapListType hashmap2(Capacity, freelistAllocator2);
	printf("Copy assign the first hashmap to the second\n");
	hashmap2 = hashmap1;
	ASSERT_EQ(hashmap2.size(), NumElements);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		ASSERT_EQ(hashmap2[i].a, i);
		ASSERT_EQ(hashmap2[i].b, i + 1);
		ASSERT_EQ(hashmap2[i].c, i + 2.0f);
		ASSERT_EQ(hashmap2[i].d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, MoveAssignHashMapList)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);
	nctl::FreeListAllocator freelistAllocator2(BufferSize_, &buffer2_);

	printf("Creating a hashmap with a custom allocator\n");
	HashMapListType hashmap1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashmap1.emplace(i, i, i + 1, i + 2.0f, i + 3.0f);

	printf("Creating a second hashmap with a different custom allocator\n");
	HashMapListType hashmap2(Capacity, freelistAllocator2);
	printf("Move assign the first hashmap to the second\n");
	hashmap2 = nctl::move(hashmap1);
	ASSERT_EQ(hashmap2.size(), NumElements);
	ASSERT_EQ(hashmap1.size(), 0);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		ASSERT_EQ(hashmap2[i].a, i);
		ASSERT_EQ(hashmap2[i].b, i + 1);
		ASSERT_EQ(hashmap2[i].c, i + 2.0f);
		ASSERT_EQ(hashmap2[i].d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, CopyConstructHashSet)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);

	printf("Creating a hashset with a custom allocator\n");
	HashSetType hashset1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashset1.insert(ElementType(i, i + 1, i + 2.0f, i + 3.0f));

	printf("Creating a second hashset with copy construction\n");
	HashSetType hashset2(hashset1);
	ASSERT_EQ(hashset2.size(), NumElements);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		const ElementType key(i, i + 1, i + 2.0f, i + 3.0f);
		const ElementType *element = hashset2.find(key);
		ASSERT_EQ(element->a, i);
		ASSERT_EQ(element->b, i + 1);
		ASSERT_EQ(element->c, i + 2.0f);
		ASSERT_EQ(element->d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, MoveConstructHashSet)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);

	printf("Creating a hashset with a custom allocator\n");
	HashSetType hashset1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashset1.insert(ElementType(i, i + 1, i + 2.0f, i + 3.0f));

	printf("Creating a second hashset with move construction\n");
	HashSetType hashset2(nctl::move(hashset1));
	ASSERT_EQ(hashset2.size(), NumElements);
	ASSERT_EQ(hashset1.size(), 0);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		const ElementType key(i, i + 1, i + 2.0f, i + 3.0f);
		const ElementType *element = hashset2.find(key);
		ASSERT_EQ(element->a, i);
		ASSERT_EQ(element->b, i + 1);
		ASSERT_EQ(element->c, i + 2.0f);
		ASSERT_EQ(element->d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, CopyAssignHashSet)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);
	nctl::FreeListAllocator freelistAllocator2(BufferSize_, &buffer2_);

	printf("Creating a hashset with a custom allocator\n");
	HashSetType hashset1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashset1.insert(ElementType(i, i + 1, i + 2.0f, i + 3.0f));

	printf("Creating a second hashset with a different custom allocator\n");
	HashSetType hashset2(Capacity, freelistAllocator2);
	printf("Copy assign the first hashset to the second\n");
	hashset2 = hashset1;
	ASSERT_EQ(hashset2.size(), NumElements);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		const ElementType key(i, i + 1, i + 2.0f, i + 3.0f);
		const ElementType *element = hashset2.find(key);
		ASSERT_EQ(element->a, i);
		ASSERT_EQ(element->b, i + 1);
		ASSERT_EQ(element->c, i + 2.0f);
		ASSERT_EQ(element->d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, MoveAssignHashSet)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);
	nctl::FreeListAllocator freelistAllocator2(BufferSize_, &buffer2_);

	printf("Creating a hashset with a custom allocator\n");
	HashSetType hashset1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashset1.insert(ElementType(i, i + 1, i + 2.0f, i + 3.0f));

	printf("Creating a second hashset with a different custom allocator\n");
	HashSetType hashset2(Capacity, freelistAllocator2);
	printf("Move assign the first hashset to the second\n");
	hashset2 = nctl::move(hashset1);
	ASSERT_EQ(hashset2.size(), NumElements);
	ASSERT_EQ(hashset1.size(), 0);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		const ElementType key(i, i + 1, i + 2.0f, i + 3.0f);
		const ElementType *element = hashset2.find(key);
		ASSERT_EQ(element->a, i);
		ASSERT_EQ(element->b, i + 1);
		ASSERT_EQ(element->c, i + 2.0f);
		ASSERT_EQ(element->d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, CopyConstructHashSetList)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);

	printf("Creating a hashset with a custom allocator\n");
	HashSetListType hashset1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashset1.insert(ElementType(i, i + 1, i + 2.0f, i + 3.0f));

	printf("Creating a second hashset with copy construction\n");
	HashSetListType hashset2(hashset1);
	ASSERT_EQ(hashset2.size(), NumElements);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		const ElementType key(i, i + 1, i + 2.0f, i + 3.0f);
		const ElementType *element = hashset2.find(key);
		ASSERT_EQ(element->a, i);
		ASSERT_EQ(element->b, i + 1);
		ASSERT_EQ(element->c, i + 2.0f);
		ASSERT_EQ(element->d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, MoveConstructHashSetList)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);

	printf("Creating a hashset with a custom allocator\n");
	HashSetListType hashset1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashset1.insert(ElementType(i, i + 1, i + 2.0f, i + 3.0f));

	printf("Creating a second hashset with move construction\n");
	HashSetListType hashset2(nctl::move(hashset1));
	ASSERT_EQ(hashset2.size(), NumElements);
	ASSERT_EQ(hashset1.size(), 0);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		const ElementType key(i, i + 1, i + 2.0f, i + 3.0f);
		const ElementType *element = hashset2.find(key);
		ASSERT_EQ(element->a, i);
		ASSERT_EQ(element->b, i + 1);
		ASSERT_EQ(element->c, i + 2.0f);
		ASSERT_EQ(element->d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, CopyAssignHashSetList)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);
	nctl::FreeListAllocator freelistAllocator2(BufferSize_, &buffer2_);

	printf("Creating a hashset with a custom allocator\n");
	HashSetListType hashset1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashset1.insert(ElementType(i, i + 1, i + 2.0f, i + 3.0f));

	printf("Creating a second hashset with a different custom allocator\n");
	HashSetListType hashset2(Capacity, freelistAllocator2);
	printf("Copy assign the first hashset to the second\n");
	hashset2 = hashset1;
	ASSERT_EQ(hashset2.size(), NumElements);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		const ElementType key(i, i + 1, i + 2.0f, i + 3.0f);
		const ElementType *element = hashset2.find(key);
		ASSERT_EQ(element->a, i);
		ASSERT_EQ(element->b, i + 1);
		ASSERT_EQ(element->c, i + 2.0f);
		ASSERT_EQ(element->d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, MoveAssignHashSetList)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);
	nctl::FreeListAllocator freelistAllocator2(BufferSize_, &buffer2_);

	printf("Creating a hashset with a custom allocator\n");
	HashSetListType hashset1(Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		hashset1.insert(ElementType(i, i + 1, i + 2.0f, i + 3.0f));

	printf("Creating a second hashset with a different custom allocator\n");
	HashSetListType hashset2(Capacity, freelistAllocator2);
	printf("Move assign the first hashset to the second\n");
	hashset2 = nctl::move(hashset1);
	ASSERT_EQ(hashset2.size(), NumElements);
	ASSERT_EQ(hashset1.size(), 0);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		const ElementType key(i, i + 1, i + 2.0f, i + 3.0f);
		const ElementType *element = hashset2.find(key);
		ASSERT_EQ(element->a, i);
		ASSERT_EQ(element->b, i + 1);
		ASSERT_EQ(element->c, i + 2.0f);
		ASSERT_EQ(element->d, i + 3.0f);
	}
}

TEST_F(AllocatorContainersTest, CopyConstructSparseSet)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);

	printf("Creating a sparseset with a custom allocator\n");
	nctl::SparseSet<unsigned int> sparseset1(NumElements, Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		sparseset1.insert(i);

	printf("Creating a second sparseset with copy construction\n");
	nctl::SparseSet<unsigned int> sparseset2(sparseset1);
	ASSERT_EQ(sparseset2.size(), NumElements);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		const unsigned int key = sparseset2.find(i);
		ASSERT_EQ(key, i);
	}
}

TEST_F(AllocatorContainersTest, MoveConstructSparseSet)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);

	printf("Creating a sparseset with a custom allocator\n");
	nctl::SparseSet<unsigned int> sparseset1(NumElements, Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		sparseset1.insert(i);

	printf("Creating a second sparseset with move construction\n");
	nctl::SparseSet<unsigned int> sparseset2(nctl::move(sparseset1));
	ASSERT_EQ(sparseset2.size(), NumElements);
	ASSERT_EQ(sparseset1.size(), 0);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		const unsigned int key = sparseset2.find(i);
		ASSERT_EQ(key, i);
	}
}

TEST_F(AllocatorContainersTest, CopyAssignSparseSet)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);
	nctl::FreeListAllocator freelistAllocator2(BufferSize_, &buffer2_);

	printf("Creating a sparseset with a custom allocator\n");
	nctl::SparseSet<unsigned int> sparseset1(NumElements, Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		sparseset1.insert(i);

	printf("Creating a second sparseset with a different custom allocator\n");
	nctl::SparseSet<unsigned int> sparseset2(NumElements, Capacity, freelistAllocator2);
	printf("Copy assign the first sparseset to the second\n");
	sparseset2 = sparseset1;
	ASSERT_EQ(sparseset2.size(), NumElements);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		const unsigned int key = sparseset2.find(i);
		ASSERT_EQ(key, i);
	}
}

TEST_F(AllocatorContainersTest, MoveAssignSparseSet)
{
	nctl::FreeListAllocator freelistAllocator1(BufferSize_, &buffer1_);
	nctl::FreeListAllocator freelistAllocator2(BufferSize_, &buffer2_);

	printf("Creating a sparseset with a custom allocator\n");
	nctl::SparseSet<unsigned int> sparseset1(NumElements, Capacity, freelistAllocator1);
	for (unsigned int i = 0; i < NumElements; i++)
		sparseset1.insert(i);

	printf("Creating a second sparseset with a different custom allocator\n");
	nctl::SparseSet<unsigned int> sparseset2(NumElements, Capacity, freelistAllocator2);
	printf("Copy assign the first sparseset to the second\n");
	sparseset2 = nctl::move(sparseset1);
	ASSERT_EQ(sparseset2.size(), NumElements);
	ASSERT_EQ(sparseset1.size(), 0);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		const unsigned int key = sparseset2.find(i);
		ASSERT_EQ(key, i);
	}
}

}
