#ifndef NCTL_ALGORITHMS
#define NCTL_ALGORITHMS

#include <cmath>
#include "iterator.h"
#include "utility.h"

namespace nctl {

///////////////////////////////////////////////////////////
// TEMPLATE FUNCTIONS (non modifying)
///////////////////////////////////////////////////////////

/// Returns the minimum between two objects that support operator<
template <class T>
inline const T &min(const T &a, const T &b)
{
	return !(b < a) ? a : b;
}

/// Returns the maximum between two objects that support operator<
template <class T>
inline const T &max(const T &a, const T &b)
{
	return (a < b) ? b : a;
}

/// Clamp the value of an object between two others that support operator<
template <class T>
inline const T &clamp(const T &value, const T &minValue, const T &maxValue)
{
	return min(max(value, minValue), maxValue);
}

///////////////////////////////////////////////////////////
// UNARY PREDICATES
///////////////////////////////////////////////////////////

/// A function object returning true if its argument is equal to a reference value sets upon object construction
template <class T>
class IsEqualTo
{
  public:
	explicit IsEqualTo(T refValue)
	    : refValue_(refValue) {}
	inline bool operator()(T value) const { return value == refValue_; }

  private:
	T refValue_;
};

/// A function object returning true if its argument is not equal to a reference value sets upon object construction
template <class T>
class IsNotEqualTo
{
  public:
	explicit IsNotEqualTo(T refValue)
	    : refValue_(refValue) {}
	inline bool operator()(T value) const { return !(value == refValue_); }

  private:
	T refValue_;
};

/// A function object returning true if its argument is greater than a reference value sets upon object construction
template <class T>
class IsGreaterThan
{
  public:
	explicit IsGreaterThan(T refValue)
	    : refValue_(refValue) {}
	inline bool operator()(T value) const { return value > refValue_; }

  private:
	T refValue_;
};

/// A function object returning true if its argument is not greater than a reference value sets upon object construction
template <class T>
class IsNotGreaterThan
{
  public:
	explicit IsNotGreaterThan(T refValue)
	    : refValue_(refValue) {}
	inline bool operator()(T value) const { return !(value > refValue_); }

  private:
	T refValue_;
};

/// A function object returning true if its argument is less than a reference value sets upon object construction
template <class T>
class IsLessThan
{
  public:
	explicit IsLessThan(T refValue)
	    : refValue_(refValue) {}
	inline bool operator()(T value) const { return value < refValue_; }

  private:
	T refValue_;
};

/// A function object returning true if its argument is not less than a reference value sets upon object construction
template <class T>
class IsNotLessThan
{
  public:
	explicit IsNotLessThan(T refValue)
	    : refValue_(refValue) {}
	inline bool operator()(T value) const { return !(value < refValue_); }

  private:
	T refValue_;
};

///////////////////////////////////////////////////////////
// BINARY PREDICATES
///////////////////////////////////////////////////////////

template <class T>
inline bool IsEqual(const T &a, const T &b)
{
	return a == b;
}

template <class T>
inline bool IsNotEqual(const T &a, const T &b)
{
	return !(a == b);
}

template <class T>
inline bool IsGreater(const T &a, const T &b)
{
	return a > b;
}

template <class T>
inline bool IsNotGreater(const T &a, const T &b)
{
	return !(a > b);
}

template <class T>
inline bool IsLess(const T &a, const T &b)
{
	return a < b;
}

template <class T>
inline bool IsNotLess(const T &a, const T &b)
{
	return !(a < b);
}

///////////////////////////////////////////////////////////
// ARITHMETIC OPERATIONS
///////////////////////////////////////////////////////////

template <class T>
inline T Plus(const T &a, const T &b)
{
	return a + b;
}

template <class T>
inline T Minus(const T &a, const T &b)
{
	return a - b;
}

template <class T>
inline T Multiplies(const T &a, const T &b)
{
	return a * b;
}

template <class T>
inline T Divides(const T &a, const T &b)
{
	return a / b;
}

template <class T>
inline T Modulus(const T &a, const T &b)
{
	return a % b;
}

template <class T>
inline T Negate(const T &a)
{
	return -a;
}

///////////////////////////////////////////////////////////
// LOGICAL OPERATIONS
///////////////////////////////////////////////////////////

template <class T>
inline T LogicalAnd(const T &a, const T &b)
{
	return a && b;
}

template <class T>
inline T LogicalOr(const T &a, const T &b)
{
	return a || b;
}

template <class T>
inline T LogicalNot(const T &a)
{
	return !a;
}

///////////////////////////////////////////////////////////
// TEMPLATE FUNCTIONS WITH ITERATORS (non-modifying)
///////////////////////////////////////////////////////////

/// Returns true if all elements in range satisfy the condition
template <class Iterator, class UnaryPredicate>
inline bool allOf(Iterator first, Iterator last, UnaryPredicate pred)
{
	for (; first != last; ++first)
	{
		if (!pred(*first))
			return false;
	}

	return true;
}

/// Returns true if no element in range satisfies the condition
template <class Iterator, class UnaryPredicate>
inline bool noneOf(Iterator first, Iterator last, UnaryPredicate pred)
{
	for (; first != last; ++first)
	{
		if (pred(*first))
			return false;
	}

	return true;
}

/// Returns true if any of the elements in range satisfies the condition
template <class Iterator, class UnaryPredicate>
inline bool anyOf(Iterator first, Iterator last, UnaryPredicate pred)
{
	for (; first != last; ++first)
	{
		if (pred(*first))
			return true;
	}

	return false;
}

/// Applies a function to each element in range
template <class Iterator, class Function>
inline Function forEach(Iterator first, Iterator last, Function fn)
{
	for (; first != last; ++first)
		fn(*first);

	return fn;
}

/// Returns an iterator to the first element in range equal to value, last element otherwise
template <class Iterator, class T>
inline Iterator find(Iterator first, Iterator last, const T &value)
{
	for (; first != last; ++first)
	{
		if (*first == value)
			return first;
	}

	return last;
}

/// Returns an iterator to the first element in range satisfying the condition, last element otherwise
template <class Iterator, class UnaryPredicate>
inline Iterator findIf(Iterator first, Iterator last, UnaryPredicate pred)
{
	for (; first != last; ++first)
	{
		if (pred(*first))
			return first;
	}

	return last;
}

/// Returns an iterator to the first element in range not satisfying the condition, last element otherwise
template <class Iterator, class UnaryPredicate>
inline Iterator findIfNot(Iterator first, Iterator last, UnaryPredicate pred)
{
	for (; first != last; ++first)
	{
		if (!pred(*first))
			return first;
	}

	return last;
}

/// Returns the number of elements in range equal to value
template <class Iterator, class T>
inline int count(Iterator first, Iterator last, const T &value)
{
	int counter = 0;

	for (; first != last; ++first)
	{
		if (*first == value)
			counter++;
	}

	return counter;
}

/// Returns the number of elements in range that satisfy the condition
template <class Iterator, class UnaryPredicate>
inline int countIf(Iterator first, Iterator last, UnaryPredicate pred)
{
	int counter = 0;

	for (; first != last; ++first)
	{
		if (pred(*first))
			counter++;
	}

	return counter;
}

/// Returns true if the elements in the two ranges are equal
template <class Iterator1, class Iterator2>
inline bool equal(Iterator1 first1, Iterator1 last1, Iterator2 first2)
{
	while (first1 != last1)
	{
		if (*first1 != *first2)
			return false;
		++first1;
		++first2;
	}

	return true;
}

/// Returns the minimum element in the range
template <class Iterator>
inline Iterator minElement(Iterator first, Iterator last)
{
	if (first == last)
		return last;

	Iterator smallest = first;
	++first;

	for (; first != last; ++first)
	{
		if (*first < *smallest)
			smallest = first;
	}

	return smallest;
}

/// Returns the maximum element in the range
template <class Iterator>
inline Iterator maxElement(Iterator first, Iterator last)
{
	if (first == last)
		return last;

	Iterator largest = first;
	++first;

	for (; first != last; ++first)
	{
		if (*first > *largest)
			largest = first;
	}

	return largest;
}

/// Clamp the value of all elements in the range
template <class Iterator, class T>
inline void clampElements(Iterator first, Iterator last, const T &minValue, const T &maxValue)
{
	for (; first != last; ++first)
		*first = min(max(*first, minValue), maxValue);
}

///////////////////////////////////////////////////////////
// TEMPLATE FUNCTIONS WITH ITERATORS (modifying)
///////////////////////////////////////////////////////////

template <class Iterator1, class Iterator2>
inline void iterSwap(Iterator1 a, Iterator2 b)
{
	swap(*a, *b);
}

/// Copies the elements from the first range into the one beginning at result
template <class IteratorIn, class IteratorOut>
inline IteratorOut copy(IteratorIn first, IteratorIn last, IteratorOut result)
{
	while (first != last)
	{
		*result = *first;
		++result;
		++first;
	}

	return result;
}

/// Copies n elements from the first iterator into the result one
template <class IteratorIn, class IteratorOut>
inline IteratorOut copyN(IteratorIn first, unsigned int n, IteratorOut result)
{
	while (n > 0)
	{
		*result = *first;
		++result;
		++first;
		--n;
	}

	return result;
}

/// Copies the elements from the first range into the one beginning at result for elements that satisfies the predicate
template <class IteratorIn, class IteratorOut, class UnaryPredicate>
inline IteratorOut copyIf(IteratorIn first, IteratorIn last, IteratorOut result, UnaryPredicate pred)
{
	while (first != last)
	{
		if (pred(*first))
		{
			*result = *first;
			++result;
		}
		++first;
	}

	return result;
}

/// Applies an operation sequentially to the elements of one range storing the result at the result iterator
template <class IteratorIn, class IteratorOut, class UnaryOperation>
inline IteratorOut transform(IteratorIn first1, IteratorIn last1, IteratorOut result, UnaryOperation op)
{
	while (first1 != last1)
	{
		*result = op(*first1);
		++result;
		++first1;
	}

	return result;
}

/// Applies an operation sequentially to the elements of two ranges storing the result at the result iterator
template <class IteratorIn1, class IteratorIn2, class IteratorOut, class BinaryOperation>
inline IteratorOut transform(IteratorIn1 first1, IteratorIn1 last1, IteratorIn2 first2,
                             IteratorOut result, BinaryOperation binaryOp)
{
	while (first1 != last1)
	{
		*result = binaryOp(*first1, *first2++);
		++result;
		++first1;
	}

	return result;
}

/// Assigns the new value to all elements of the range that are equal to the specified value
template <class Iterator, class T>
inline void replace(Iterator first, Iterator last, const T &oldValue, const T &newValue)
{
	while (first != last)
	{
		if (*first == oldValue)
			*first = newValue;
		++first;
	}
}

/// Assigns the new value to all elements of the range that are equal to the specified value
template <class Iterator, class UnaryPredicate, class T>
inline void replaceIf(Iterator first, Iterator last, UnaryPredicate pred, const T &newValue)
{
	while (first != last)
	{
		if (pred(*first))
			*first = newValue;
		++first;
	}
}

/// Copies the elements from the first range into the one beginning at result, replacing the appearances of the specified value with the new one
template <class IteratorIn, class IteratorOut, class T>
inline IteratorOut replaceCopy(IteratorIn first, IteratorIn last, IteratorOut result, const T &oldValue, const T &newValue)
{
	while (first != last)
	{
		*result = (*first == oldValue) ? newValue : *first;
		++first;
		++result;
	}

	return result;
}

/// Copies the elements from the first range into the one beginning at result, replacing those that satisfy the predicate with the new value
template <class IteratorIn, class IteratorOut, class UnaryPredicate, class T>
inline IteratorOut replaceCopyIf(IteratorIn first, IteratorIn last, IteratorOut result, UnaryPredicate pred, const T &newValue)
{
	while (first != last)
	{
		*result = (pred(*first)) ? newValue : *first;
		++first;
		++result;
	}

	return result;
}

/// Fill a range with value
template <class Iterator, class T>
inline void fill(Iterator first, Iterator last, const T &value)
{
	for (; first != last; ++first)
		*first = value;
}

/// Fill a number of elements of a container with a value
template <class Iterator, class T>
inline void fillN(Iterator first, unsigned int n, const T &value)
{
	for (unsigned int i = 0; i < n; i++, ++first)
		*first = value;
}

/// Assigns the values returned by the generator to the elements in the range
template <class Iterator, class Generator>
inline void generate(Iterator first, Iterator last, Generator gen)
{
	while (first != last)
	{
		*first = gen();
		++first;
	}
}

/// Assigns the values returned by the generator to the first n elements pointed by the iterator
template <class Iterator, class Generator>
inline void generateN(Iterator first, unsigned int n, Generator gen)
{
	while (n > 0)
	{
		*first = gen();
		++first;
		--n;
	}
}

/// Removes all the elements equal to the value, and returns an iterator to the new end of that range
template <class Iterator, class T>
inline Iterator remove(Iterator first, Iterator last, const T &val)
{
	Iterator result = first;

	while (first != last)
	{
		if (!(*first == val))
		{
			*result = *first;
			++result;
		}
		++first;
	}

	return result;
}

/// Removes all the elements in the range for which the predicate is true, and returns an iterator to the new end of that range
template <class Iterator, class UnaryPredicate>
inline Iterator removeIf(Iterator first, Iterator last, UnaryPredicate pred)
{
	Iterator result = first;

	while (first != last)
	{
		if (!pred(*first))
		{
			*result = *first;
			++result;
		}
		++first;
	}

	return result;
}

/// Reverses the order of the elements in the range
template <class Iterator>
inline void reverse(Iterator first, Iterator last)
{
	while ((first != last) && (first != --last))
	{
		iterSwap(first, last);
		++first;
	}
}

/// Copies the elements in the range at result, but in reverse order
template <class IteratorIn, class IteratorOut>
inline IteratorOut reverseCopy(IteratorIn first, IteratorIn last, IteratorOut result)
{
	while (first != last)
	{
		--last;
		*result = *last;
		++result;
	}

	return result;
}

/// Returns true if the range is sorted into ascending order
template <class Iterator>
inline bool isSorted(Iterator first, Iterator last)
{
	if (first == last)
		return true;

	Iterator next = first;
	while (++next != last)
	{
		if (*next < *first)
			return false;
		++first;
	}

	return true;
}

/// Returns true if the range is sorted, using a custom comparison
template <class Iterator, class Compare>
inline bool isSorted(Iterator first, Iterator last, Compare comp)
{
	if (first == last)
		return true;

	Iterator next = first;
	while (++next != last)
	{
		if (comp(*next, *first))
			return false;
		++first;
	}

	return true;
}

/// Returns an iterator to the first element in the range which does not follow an ascending order, or last if sorted
template <class Iterator>
inline Iterator isSortedUntil(Iterator first, Iterator last)
{
	if (first == last)
		return first;

	Iterator next = first;
	while (++next != last)
	{
		if (*next < *first)
			return next;
		++first;
	}

	return last;
}

/// Returns an iterator to the first element in the range which does not follow the custom comparison, or last if sorted
template <class Iterator, class Compare>
inline Iterator isSortedUntil(Iterator first, Iterator last, Compare comp)
{
	if (first == last)
		return first;

	Iterator next = first;
	while (++next != last)
	{
		if (comp(*next, *first))
			return next;
		++first;
	}

	return last;
}

namespace {

	/// Chooses the median between three values, to avoid unbalanced partitions with already sorted arrays
	template <class Iterator, class Compare>
	Iterator medianOfThree(Iterator a, Iterator b, Iterator c, Compare comp)
	{
		if (comp(*a, *b))
		{
			if (comp(*b, *c)) return b; // a < b < c
			else if (comp(*a, *c)) return c; // a < c <= b
			else return a; // c <= a < b
		}
		else
		{
			if (comp(*a, *c)) return a; // b <= a < c
			else if (comp(*b, *c)) return c; // b < c <= a
			else return b; // c <= b <= a
		}
	}

	/// Lomuto partition function for quicksort with iterators and custom compare function
	template <class Iterator, class Compare>
	inline Iterator partition(Iterator first, Iterator last, Compare comp)
	{
		Iterator pivot = last;
		Iterator i = first;
		for (Iterator j = first; j != last; ++j)
		{
			if (comp(*j, *pivot))
			{
				swap(*i, *j);
				++i;
			}
		}

		swap(*i, *pivot);
		return i;
	}

	/// Quicksort implementation with random access iterators and custom compare function
	template <class Iterator, class Compare>
	inline void quicksort(Iterator first, Iterator last, RandomAccessIteratorTag, Compare comp)
	{
		const int size = distance(first, last);
		if (size > 1)
		{
			Iterator lastElem = prev(last);
			Iterator mid = next(first, size / 2);
			Iterator pivot = medianOfThree(first, mid, lastElem, comp);
			swap(*pivot, *lastElem);
			Iterator q = partition(first, lastElem, comp);

			quicksort(first, q, RandomAccessIteratorTag(), comp);
			quicksort(next(q), last, RandomAccessIteratorTag(), comp);
		}
	}

	/// Quicksort implementation with bidirectional iterators and custom compare function
	template <class Iterator, class Compare>
	inline void quicksort(Iterator first, Iterator last, BidirectionalIteratorTag, Compare comp)
	{
		if (first != last)
		{
			Iterator p = prev(last);
			swap(*first, *p);
			Iterator q = partition(first, p, comp);
			swap(*q, *p);

			quicksort(first, q, BidirectionalIteratorTag(), comp);
			quicksort(next(q), last, BidirectionalIteratorTag(), comp);
		}
	}

}

/// Quicksort implementation with iterators and custom compare function
template <class Iterator, class Compare>
inline void quicksort(Iterator first, Iterator last, Compare comp)
{
	quicksort(first, last, typename IteratorTraits<Iterator>::IteratorCategory(), comp);
}

/// Quicksort implementation with iterators, ascending order
template <class Iterator>
inline void quicksort(Iterator first, Iterator last)
{
	quicksort(first, last, typename IteratorTraits<Iterator>::IteratorCategory(), IsLess<typename IteratorTraits<Iterator>::ValueType>);
}

/// Quicksort implementation with iterators, descending order
template <class Iterator>
inline void quicksortDesc(Iterator first, Iterator last)
{
	quicksort(first, last, typename IteratorTraits<Iterator>::IteratorCategory(), IsNotLess<typename IteratorTraits<Iterator>::ValueType>);
}

namespace {

	/// Heapsort utility function to return the index of the left child of the heap
	inline int iLeftChild(int i)
	{
		return (2 * i + 1);
	}

}

/// Heapsort implementation with iterators and custom compare function
template <class Iterator, class Compare>
inline void heapsort(Iterator first, Iterator last, Compare comp)
{
	const int size = distance(first, last);
	int start = size / 2;
	int end = size;

	while (end > 1)
	{
		if (start > 0)
			start--;
		else
		{
			end--;
			swap(*(first + end), *first);
		}

		int root = start;
		while (iLeftChild(root) < end)
		{
			int child = iLeftChild(root);
			if (child + 1 < end && comp(*(first + child), *(first + child + 1)))
				child++;

			if (comp(*(first + root), *(first + child)))
			{
				swap(*(first + root), *(first + child));
				root = child;
			}
			else
				break;
		}
	}
}

/// Heapsort implementation with iterators, ascending order
template <class Iterator>
inline void heapsort(Iterator first, Iterator last)
{
	heapsort(first, last, IsLess<typename IteratorTraits<Iterator>::ValueType>);
}

/// Heapsort implementation with iterators, descending order
template <class Iterator>
inline void heapsortDesc(Iterator first, Iterator last)
{
	heapsort(first, last, IsNotLess<typename IteratorTraits<Iterator>::ValueType>);
}

/// Insertion sort implementation with iterators and custom compare function
template <class Iterator, class Compare>
inline void insertionsort(Iterator first, Iterator last, Compare comp)
{
	const int size = distance(first, last);

	for (int i = 1; i < size; i++)
	{
		const auto x = *(first + i);
		int j = i;

		while (j > 0 && comp(x, *(first + j - 1)))
		{
			*(first + j) = *(first + j - 1);
			j--;
		}
		*(first + j) = x;
	}
}

/// Insertion sort implementation with iterators, ascending order
template <class Iterator>
inline void insertionsort(Iterator first, Iterator last)
{
	insertionsort(first, last, IsLess<typename IteratorTraits<Iterator>::ValueType>);
}

/// Insertion sort implementation with iterators, descending order
template <class Iterator>
inline void insertionsortDesc(Iterator first, Iterator last)
{
	insertionsort(first, last, IsNotLess<typename IteratorTraits<Iterator>::ValueType>);
}

namespace {

	/// Introspective sort implementation with iterators and custom compare function
	template <class Iterator, class Compare>
	inline void introsort(Iterator first, Iterator last, Compare comp, unsigned int maxDepth)
	{
		const int size = distance(first, last);
		if (size < 16)
			insertionsort(first, last, comp);
		else if (maxDepth == 0)
			heapsort(first, last, comp);
		else
		{
			Iterator lastElem = prev(last);
			Iterator mid = next(first, size / 2);
			Iterator p = medianOfThree(first, mid, lastElem, comp);
			swap(*p, *lastElem);
			Iterator q = partition(first, lastElem, comp);

			introsort(first, q, comp, maxDepth - 1);
			introsort(next(q), last, comp, maxDepth - 1);
		}
	}

}

/// Default sort implementation using introsort, with iterators and custom compare function
template <class Iterator, class Compare>
inline void sort(Iterator first, Iterator last, Compare comp)
{
	const unsigned int maxDepth = floor(log2(max(1, distance(first, last)))) * 2;
	introsort(first, last, comp, maxDepth);
}

/// Default sort implementation using introsort with iterators, ascending order
template <class Iterator>
inline void sort(Iterator first, Iterator last)
{
	const unsigned int maxDepth = floor(log2(max(1, distance(first, last)))) * 2;
	introsort(first, last, IsLess<typename IteratorTraits<Iterator>::ValueType>, maxDepth);
}

/// Default sort implementation using introsort with iterators, descending order
template <class Iterator>
inline void sortDesc(Iterator first, Iterator last)
{
	const unsigned int maxDepth = floor(log2(max(1, distance(first, last)))) * 2;
	introsort(first, last, IsNotLess<typename IteratorTraits<Iterator>::ValueType>, maxDepth);
}

}

#endif
