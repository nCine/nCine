#include "gtest_span.h"

namespace {

class SpanTest : public ::testing::Test
{
  public:
	SpanTest()
	    : span_(array_, Capacity) {}

  protected:
	const int array_[Capacity] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	SpanType span_;
};

#ifndef __EMSCRIPTEN__
	#ifdef NCINE_DEBUG
TEST(SpanDeathTest, SubscriptAccessBeyondSize)
{
	const int array[1] = { 0 };
	printf("Trying to access an element of a span beyond size\n");
	SpanType span(array);

	ASSERT_DEATH(span[5], "");
}

TEST(SpanDeathTest, SubscriptAccessAtSize)
{
	const int array[1] = { 0 };
	printf("Trying to access an element of a span at size\n");
	SpanType span(array);

	ASSERT_DEATH(span[1], "");
}

TEST(SpanDeathTest, SubSpanFirstBeyondSize)
{
	const int array[5] = { 0, 1, 2, 3, 4 };
	SpanType span(array);

	ASSERT_DEATH(span.first(6), "");
}

TEST(SpanDeathTest, SubSpanLastBeyondSize)
{
	const int array[5] = { 0, 1, 2, 3, 4 };
	SpanType span(array);

	ASSERT_DEATH(span.last(6);, "");
}

TEST(SpanDeathTest, SubSpanMiddleOffsetBeyondSize)
{
	const int array[5] = { 0, 1, 2, 3, 4 };
	SpanType span(array);

	ASSERT_DEATH(span.subSpan(6, 1);, "");
}

TEST(SpanDeathTest, SubSpanMiddleCountBeyondSize)
{
	const int array[5] = { 0, 1, 2, 3, 4 };
	SpanType span(array);

	ASSERT_DEATH(span.subSpan(0, 6);, "");
}
	#endif

TEST(SpanDeathTest, AccessBeyondSize)
{
	const int array[1] = { 0 };
	printf("Trying to access an element of a span beyond size\n");
	SpanType span(array);

	ASSERT_DEATH(span.at(5), "");
}

TEST(SpanDeathTest, AccessAtSize)
{
	const int array[1] = { 0 };
	printf("Trying to access an element of a span at size\n");
	SpanType span(array);

	ASSERT_DEATH(span.at(1), "");
}

TEST(SpanDeathTest, FrontElementFromEmptySpan)
{
	SpanType span;
	printf("Retrieving the front element from an empty span\n");

	ASSERT_EQ(span.size(), 0);
	ASSERT_DEATH(span.front(), "");
}

TEST(SpanDeathTest, BackElementFromEmptySpan)
{
	SpanType span;
	printf("Retrieving the back element from an empty span\n");

	ASSERT_EQ(span.size(), 0);
	ASSERT_DEATH(span.back(), "");
}
#endif

TEST_F(SpanTest, DefaultConstructor)
{
	printf("Constructing an empty span\n");
	SpanType span;
	printSpan(span);

	ASSERT_TRUE(span.isEmpty());
	ASSERT_EQ(span.size(), 0);
	ASSERT_EQ(span.data(), nullptr);
	ASSERT_EQ(span.sizeBytes(), 0);
}

TEST_F(SpanTest, ConstructorFromArray)
{
	printf("Constructing a span from an array\n");
	SpanType span(array_);
	printSpan(span);

	ASSERT_FALSE(span.isEmpty());
	ASSERT_EQ(span.size(), Capacity);
	ASSERT_EQ(span.data(), array_);
	ASSERT_EQ(span.sizeBytes(), Capacity * sizeof(int));
}

TEST_F(SpanTest, ConstructorFromPointerAndSize)
{
	printf("Constructing a span from a pointer and a size\n");
	SpanType span(array_, Capacity);
	printSpan(span);

	ASSERT_FALSE(span.isEmpty());
	ASSERT_EQ(span.size(), Capacity);
	ASSERT_EQ(span.data(), array_);
	ASSERT_EQ(span.sizeBytes(), Capacity * sizeof(int));
}

TEST_F(SpanTest, ConstructorFromDifferentType)
{
	int newArray[Capacity] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	nctl::Span<int> newSpan(newArray);
	printf("Constructing a span from a different span type\n");
	SpanType span(newSpan);
	printSpan(span);

	ASSERT_FALSE(span.isEmpty());
	ASSERT_EQ(span.size(), Capacity);
	ASSERT_EQ(span.data(), newArray);
	ASSERT_EQ(span.sizeBytes(), Capacity * sizeof(int));
}

TEST_F(SpanTest, ConstructorFromPointers)
{
	printf("Constructing a span from two pointers\n");
	SpanType span(&array_[3], &array_[6]);
	printSpan(span);

	ASSERT_FALSE(span.isEmpty());
	ASSERT_EQ(span.size(), 3);
	ASSERT_EQ(span.data(), &array_[3]);
	ASSERT_EQ(span.sizeBytes(), 3 * sizeof(int));
}

TEST_F(SpanTest, ConstructorFromIterators)
{
	printf("Constructing a span from two iterators\n");
	SpanType span(nctl::ArrayIterator<const int, false>(array_ + 3),
	              nctl::ArrayIterator<const int, false>(array_ + 6));
	printSpan(span);

	ASSERT_FALSE(span.isEmpty());
	ASSERT_EQ(span.size(), 3);
	ASSERT_EQ(span.data(), &array_[3]);
	ASSERT_EQ(span.sizeBytes(), 3 * sizeof(int));
}

TEST_F(SpanTest, ConstructorFromConstIterators)
{
	printf("Constructing a span from two iterators\n");
	SpanType span(nctl::ArrayIterator<const int, true>(array_ + 3),
	              nctl::ArrayIterator<const int, true>(array_ + 6));
	printSpan(span);

	ASSERT_FALSE(span.isEmpty());
	ASSERT_EQ(span.size(), 3);
	ASSERT_EQ(span.data(), &array_[3]);
	ASSERT_EQ(span.sizeBytes(), 3 * sizeof(int));
}

TEST_F(SpanTest, FrontElement)
{
	const int frontElement = span_.front();
	printf("Front element: %d\n", frontElement);

	ASSERT_EQ(frontElement, 0);
	ASSERT_EQ(frontElement, span_[0]);
	ASSERT_EQ(frontElement, array_[0]);
}

TEST_F(SpanTest, BackElement)
{
	const int backElement = span_.back();
	printf("Back element: %d\n", backElement);

	ASSERT_EQ(backElement, Capacity - 1);
	ASSERT_EQ(backElement, span_[Capacity - 1]);
	ASSERT_EQ(backElement, array_[Capacity - 1]);
}

TEST_F(SpanTest, AccessWithinSize)
{
	printf("Accessing an element within the span size\n");

	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(span_.at(i), i);
}

TEST_F(SpanTest, SubscriptAccessWithinSize)
{
	printf("Accessing an element within the span size\n");

	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(span_[i], i);
}

TEST_F(SpanTest, ConstGetters)
{
	const int *data = span_.data();
	const size_t size = span_.size();
	const size_t sizeBytes = span_.sizeBytes();
	const bool isEmpty = span_.isEmpty();

	printf("Span %s empty has %lu elements (%lu bytes) at address 0x%lx\n",
	       isEmpty ? "is" : "is not", size, sizeBytes, uintptr_t(data));
	printSpan(span_);

	ASSERT_EQ(data, array_);
	ASSERT_EQ(size, Capacity);
	ASSERT_EQ(sizeBytes, Capacity * sizeof(int));
	ASSERT_FALSE(isEmpty);
}

TEST_F(SpanTest, SubSpanFirst)
{
	const unsigned int SubSpanCount = 4;
	printf("Constructing a sub-span from another span\n");
	SpanType span = span_.first(SubSpanCount);
	printSpan(span);

	for (unsigned int i = 0; i < span.size(); i++)
		ASSERT_EQ(span_[i], span[i]);

	ASSERT_FALSE(span.isEmpty());
	ASSERT_EQ(span.size(), SubSpanCount);
	ASSERT_EQ(span.data(), &array_[0]);
	ASSERT_EQ(span.sizeBytes(), SubSpanCount * sizeof(int));
}

TEST_F(SpanTest, SubSpanLast)
{
	const unsigned int SubSpanCount = 4;
	printf("Constructing a sub-span from another span\n");
	SpanType span = span_.last(SubSpanCount);
	printSpan(span);

	for (unsigned int i = 0; i < span.size(); i++)
		ASSERT_EQ(span_[i + (Capacity - SubSpanCount)], span[i]);

	ASSERT_FALSE(span.isEmpty());
	ASSERT_EQ(span.size(), SubSpanCount);
	ASSERT_EQ(span.data(), &array_[Capacity - SubSpanCount]);
	ASSERT_EQ(span.sizeBytes(), SubSpanCount * sizeof(int));
}

TEST_F(SpanTest, SubSpanMiddle)
{
	const unsigned int SubSpanOffset = 3;
	const unsigned int SubSpanCount = 4;
	printf("Constructing a sub-span from another span\n");
	SpanType span = span_.subSpan(SubSpanOffset, SubSpanCount);
	printSpan(span);

	for (unsigned int i = 0; i < span.size(); i++)
		ASSERT_EQ(span_[i + SubSpanOffset], span[i]);

	ASSERT_FALSE(span.isEmpty());
	ASSERT_EQ(span.size(), SubSpanCount);
	ASSERT_EQ(span.data(), &array_[SubSpanOffset]);
	ASSERT_EQ(span.sizeBytes(), SubSpanCount * sizeof(int));
}

}
