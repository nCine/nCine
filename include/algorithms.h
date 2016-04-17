#ifndef NCINE_ALGORITHMS
#define NCINE_ALGORITHMS

#include <cstdlib> // for rand()
#include "iterator_traits.h"

namespace ncine {

///////////////////////////////////////////////////////////
// TEMPLATE FUNCTIONS (non modifying)
///////////////////////////////////////////////////////////

/// Returns the minimum between two objects that support operator<
template <class T>
inline const T& min(const T& a, const T& b)
{
	return !(b < a) ? a : b;
}

/// Returns the maximum between two objects that support operator<
template<class T>
inline const T& max(const T& a, const T& b)
{
	return (a < b) ? b : a;
}

/// Returns a random float between x0 and x1
inline float randBetween(float x0, float x1)
{
	return x0 + (x1 - x0) * (rand() / static_cast<float>(RAND_MAX));
}

///////////////////////////////////////////////////////////
// TEMPLATE FUNCTIONS (modifying)
///////////////////////////////////////////////////////////

/// Swaps the content of two objects of the same type
template <class T>
inline void swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}

/// Clamp the value of an object between two others that support operator<
template<class T>
inline const T& clamp(const T& value, const T& minValue, const T& maxValue)
{
	return min(max(value, minValue), maxValue);
}

///////////////////////////////////////////////////////////
// UNARY PREDICATES
///////////////////////////////////////////////////////////

template <class T>
class IsEqualTo
{
  public:
	explicit IsEqualTo(T refValue) : refValue_(refValue) { }
	inline bool operator()(T value) const { return value == refValue_; }

  private:
	T refValue_;
};

template <class T>
class IsNotEqualTo
{
  public:
	explicit IsNotEqualTo(T refValue) : refValue_(refValue) { }
	inline bool operator()(T value) const { return !(value == refValue_); }

  private:
	T refValue_;
};

template <class T>
class IsGreaterThan
{
  public:
	explicit IsGreaterThan(T refValue) : refValue_(refValue) { }
	inline bool operator()(T value) const { return value > refValue_; }

  private:
	T refValue_;
};

template <class T>
class IsNotGreaterThan
{
  public:
	explicit IsNotGreaterThan(T refValue) : refValue_(refValue) { }
	inline bool operator()(T value) const { return !(value > refValue_); }

  private:
	T refValue_;
};

template <class T>
class IsLessThan
{
  public:
	explicit IsLessThan(T refValue) : refValue_(refValue) { }
	inline bool operator()(T value) const { return value < refValue_; }

  private:
	T refValue_;
};

template <class T>
class IsNotLessThan
{
  public:
	explicit IsNotLessThan(T refValue) : refValue_(refValue) { }
	inline bool operator()(T value) const { return !(value < refValue_); }

  private:
	T refValue_;
};

///////////////////////////////////////////////////////////
// BINARY PREDICATES
///////////////////////////////////////////////////////////

template <class T>
inline bool IsEqual(const T& a, const T& b) { return a == b; }

template <class T>
inline bool IsNotEqual(const T& a, const T& b) { return !(a == b); }

template <class T>
inline bool IsGreater(const T& a, const T& b) { return a > b; }

template <class T>
inline bool IsNotGreater(const T& a, const T& b) { return !(a < b); }

template <class T>
inline bool IsLess(const T& a, const T& b) { return a < b; }

template <class T>
inline bool IsNotLess(const T& a, const T& b) { return !(a < b); }

///////////////////////////////////////////////////////////
// ARITHMETIC OPERATIONS
///////////////////////////////////////////////////////////

template <class T>
inline T Plus(const T& a, const T& b) { return a + b; }

template <class T>
inline T Minus(const T& a, const T& b) { return a - b; }

template <class T>
inline T Multiplies(const T& a, const T& b) { return a * b; }

template <class T>
inline T Divides(const T& a, const T& b) { return a / b; }

template <class T>
inline T Modulus(const T& a, const T& b) { return a % b; }

template <class T>
inline T Negate(const T& a) { return -a; }

///////////////////////////////////////////////////////////
// LOGICAL OPERATIONS
///////////////////////////////////////////////////////////

template <class T>
inline T LogicalAnd(const T& a, const T& b) { return a && b; }

template <class T>
inline T LogicalOr(const T& a, const T& b) { return a || b; }

template <class T>
inline T LogicalNot(const T& a) { return !a; }

///////////////////////////////////////////////////////////
// TEMPLATE FUNCTIONS WITH ITERATORS (non-modifying)
///////////////////////////////////////////////////////////

namespace {

/// Increments an iterator by n elements, for random access iterators
template <class Iterator>
inline void advance(Iterator& it, int n, RandomAccessIteratorTag)
{
	it += n;
}

/// Increments an iterator by n elements, for bidirectional iterators
template <class Iterator>
inline void advance(Iterator& it, int n, BidirectionalIteratorTag)
{
	if (n < 0)
	{
		while (n++) { --it; }
	}
	else
	{
		while (n--) { ++it; }
	}
}

/// Increments an iterator by n elements, for forward iterators
template <class Iterator>
inline void advance(Iterator& it, int n, ForwardIteratorTag)
{
	if (n > 0)
	{
		while (n--) { ++it; }
	}
}

}

/// Increments an iterator by n elements
template <class Iterator>
inline void advance(Iterator& it, int n)
{
	advance(it, n, IteratorTraits<Iterator>::IteratorCategory());
}

/// Return the nth successor of an iterator
template <class Iterator>
inline Iterator next(Iterator it, unsigned int n)
{
	advance(it, n);
	return it;
}

/// Return the successor of an iterator
template <class Iterator>
inline Iterator next(Iterator it)
{
	advance(it, 1);
	return it;
}

/// Return the nth predecessor of an iterator
template <class Iterator>
inline Iterator prev(Iterator it, unsigned int n)
{
	advance(it, -n);
	return it;
}

/// Return the predecessor of an iterator
template <class Iterator>
inline Iterator prev(Iterator it)
{
	advance(it, -1);
	return it;
}

namespace {

/// Returns the distance between two random access iterators with a pointer subtraction
template <class RandomAccessIterator>
inline int distance(RandomAccessIterator& first, const RandomAccessIterator& last, RandomAccessIteratorTag)
{
	return last - first;
}

/// Returns the distance in number of increments between two forward iterators
template <class ForwardIterator>
inline int distance(ForwardIterator& first, const ForwardIterator& last, ForwardIteratorTag)
{
	int counter = 0;

	for (; first != last; ++first)
	{
		counter++;
	}

	return counter;
}

}

/// Returns the distance between two iterators
template <class Iterator>
inline int distance(Iterator first, const Iterator last)
{
	return distance(first, last, IteratorTraits<Iterator>::IteratorCategory());
}

/// Returns true if all elements in range satisfy the condition
template <class Iterator, class UnaryPredicate>
inline bool allOf(Iterator first, const Iterator last, UnaryPredicate pred)
{
	for (; first != last; ++first)
	{
		if (!pred(*first)) { return false; }
	}

	return true;
}

/// Returns true if no element in range satisfies the condition
template <class Iterator, class UnaryPredicate>
inline bool noneOf(Iterator first, const Iterator last, UnaryPredicate pred)
{
	for (; first != last; ++first)
	{
		if (pred(*first)) { return false; }
	}

	return true;
}

/// Returns true if any of the elements in range satisfies the condition
template <class Iterator, class UnaryPredicate>
inline bool anyOf(Iterator first, const Iterator last, UnaryPredicate pred)
{
	for (; first != last; ++first)
	{
		if (pred(*first)) { return true; }
	}

	return false;
}

/// Applies a function to each element in range
template<class Iterator, class Function>
Function forEach(Iterator first, const Iterator last, Function fn)
{
	for (; first != last; ++first)
	{
		fn(*first);
	}

	return fn;
}

/// Returns an iterator to the first element in range equal to value, last element otherwise
template<class Iterator, class T>
Iterator find(Iterator first, const Iterator last, const T& value)
{
	for (; first != last; ++first)
	{
		if (*first == value) { return first; }
	}

	return last;
}

/// Returns an iterator to the first element in range satisfying the condition, last element otherwise
template<class Iterator, class UnaryPredicate>
Iterator findIf(Iterator first, const Iterator last, UnaryPredicate pred)
{
	for (; first != last; ++first)
	{
		if (pred(*first)) { return first; }
	}

	return last;
}

/// Returns an iterator to the first element in range not satisfying the condition, last element otherwise
template<class Iterator, class UnaryPredicate>
Iterator findIfNot(Iterator first, const Iterator last, UnaryPredicate pred)
{
	for (; first != last; ++first)
	{
		if (!pred(*first)) { return first; }
	}

	return last;
}

/// Returns the number of elements in range equal to value
template <class Iterator, class T>
inline int count(Iterator first, const Iterator last, const T& value)
{
	int counter = 0;

	for (; first != last; ++first)
	{
		if (*first == value) { counter++; }
	}

	return counter;
}

/// Returns the number of elements in range that satisfy the condition
template <class Iterator, class UnaryPredicate>
inline int countIf(Iterator first, const Iterator last, UnaryPredicate pred)
{
	int counter = 0;

	for (; first != last; ++first)
	{
		if (pred(*first)) { counter++; }
	}

	return counter;
}

/// Returns true if the elements in the two ranges are equal
template <class Iterator1, class Iterator2>
inline bool equal(Iterator1 first1, const Iterator1 last1, Iterator2 first2)
{
	while (first1 != last1)
	{
		if (*first1 != *first2)
		{
			return false;
		}
		++first1; ++first2;
	}
	return true;
}

/// Returns the minimum element in the range
template <class Iterator>
inline Iterator minElement(Iterator first, const Iterator last)
{
	if (first == last) { return last; }
	Iterator smallest = first;
	++first;

	for (; first != last; ++first)
	{
		if (*first < *smallest)
		{
			smallest = first;
		}
	}

	return smallest;
}

/// Returns the maximum element in the range
template <class Iterator>
inline Iterator maxElement(Iterator first, const Iterator last)
{
	if (first == last) { return last; }
	Iterator largest = first;
	++first;

	for (; first != last; ++first)
	{
		if (*first > *largest)
		{
			largest = first;
		}
	}

	return largest;
}

/// Clamp the value of all elements in the range
template<class Iterator, class T>
inline const T& clampElements(Iterator first, const Iterator last, const T& minValue, const T& maxValue)
{
	for (; first != last; ++first)
	{
		*first = min(max(*first, minValue), maxValue);
	}
}

///////////////////////////////////////////////////////////
// TEMPLATE FUNCTIONS WITH ITERATORS (modifying)
///////////////////////////////////////////////////////////

template<class Iterator1, class Iterator2>
void iterSwap(Iterator1 a, Iterator2 b)
{
	swap(*a, *b);
}

/// Copies the elements from the first range into the one beginning at result
template <class IteratorIn, class IteratorOut>
inline IteratorOut copy(IteratorIn first, const IteratorIn last, IteratorOut result)
{
	while (first != last)
	{
		*result = *first;
		++result; ++first;
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
		++result; ++first;
		--n;
	}
	return result;
}

/// Copies the elements from the first range into the one beginning at result for elements that satisfies the predicate
template <class IteratorIn, class IteratorOut, class UnaryPredicate>
inline IteratorOut copyIf(IteratorIn first, const IteratorIn last, IteratorOut result, UnaryPredicate pred)
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
inline IteratorOut transform(IteratorIn first1, const IteratorIn last1, IteratorOut result, UnaryOperation op)
{
	while (first1 != last1)
	{
		*result = op(*first1);
		++result; ++first1;
	}
	return result;
}

/// Applies an operation sequentially to the elements of two ranges storing the result at the result iterator
template <class IteratorIn1, class IteratorIn2, class IteratorOut, class BinaryOperation>
inline IteratorOut transform(IteratorIn1 first1, const IteratorIn1 last1, IteratorIn2 first2,
							 IteratorOut result, BinaryOperation binaryOp)
{
	while (first1 != last1)
	{
		*result = binaryOp(*first1, *first2++);
		++result; ++first1;
	}
	return result;
}

/// Assigns the new value to all elements of the range that are equal to the specified value
template <class Iterator, class T>
void replace(Iterator first, const Iterator last, const T& oldValue, const T& newValue)
{
	while (first != last)
	{
		if (*first == oldValue)
		{
			*first = newValue;
		}
		++first;
	}
}

/// Assigns the new value to all elements of the range that are equal to the specified value
template <class Iterator, class UnaryPredicate, class T>
void replaceIf(Iterator first, const Iterator last, UnaryPredicate pred, const T& newValue)
{
	while (first != last)
	{
		if (pred(*first))
		{
			*first = newValue;
		}
		++first;
	}
}

/// Copies the elements from the first range into the one beginning at result, replacing the appearances of the specified value with the new one
template <class IteratorIn, class IteratorOut, class T>
IteratorOut replaceCopy(IteratorIn first, const IteratorIn last, IteratorOut result, const T& oldValue, const T& newValue)
{
	while (first != last)
	{
		*result = (*first == oldValue) ? newValue : *first;
		++first; ++result;
	}
	return result;
}

/// Copies the elements from the first range into the one beginning at result, replacing those that satisfy the predicate with the new value
template <class IteratorIn, class IteratorOut, class UnaryPredicate, class T>
IteratorOut replaceCopyIf(IteratorIn first, const IteratorIn last, IteratorOut result, UnaryPredicate pred, const T& newValue)
{
	while (first != last)
	{
		*result = (pred(*first)) ? newValue : *first;
		++first; ++result;
	}
	return result;
}

/// Fill a range with value
template <class Iterator, class T>
inline void fill(Iterator first, const Iterator last, const T& value)
{
	for (; first != last; ++first)
	{
		*first = value;
	}
}

/// Fill a number of elements of a container with a value
template <class Iterator, class T>
inline void fillN(Iterator first, unsigned int n, const T& value)
{
	for (unsigned int i = 0; i < n; i++, ++first)
	{
		*first = value;
	}
}

/// Assigns the values returned by the generator to the elements in the range
template <class Iterator, class Generator>
void generate(Iterator first, const Iterator last, Generator gen )
{
	while (first != last)
	{
		*first = gen();
		++first;
	}
}

/// Assigns the values returned by the generator to the first n elements pointed by the iterator
template <class Iterator, class Generator>
void generateN(Iterator first, unsigned int n, Generator gen )
{
	while (n > 0)
	{
		*first = gen();
		++first; --n;
	}
}

/// Removes all the elements equal to the value, and returns an iterator to the new end of that range
template <class Iterator, class T>
Iterator remove(Iterator first, const Iterator last, const T& val)
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
Iterator removeIf(Iterator first, const Iterator last, UnaryPredicate pred)
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
void reverse (Iterator first, Iterator last)
{
	while ((first != last) && (first != --last))
	{
		iterSwap(first, last);
		++first;
	}
}

/// Copies the elements in the range at result, but in reverse order
template <class IteratorIn, class IteratorOut>
IteratorOut reverseCopy (IteratorIn first, IteratorIn last, IteratorOut result)
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
bool isSorted(Iterator first, const Iterator last)
{
	if (first == last) { return true; }

	Iterator next = first;
	while (++next != last)
	{
		if (*next < *first)
		{
			return false;
		}
		++first;
	}
	return true;
}

/// Returns true if the range is sorted, using a custom comparison
template <class Iterator, class Compare>
bool isSorted(Iterator first, const Iterator last, Compare comp)
{
	if (first == last) { return true; }

	Iterator next = first;
	while (++next != last)
	{
		if (comp(*next, *first))
		{
			return false;
		}
		++first;
	}
	return true;
}

/// Returns an iterator to the first element in the range which does not follow an ascending order, or last if sorted
template <class Iterator>
bool isSortedUntil(Iterator first, const Iterator last)
{
	if (first == last) { return first; }

	Iterator next = first;
	while (++next != last)
	{
		if (*next < *first)
		{
			return next;
		}
		++first;
	}
	return last;
}

/// Returns an iterator to the first element in the range which does not follow the custom comparison, or last if sorted
template <class Iterator, class Compare>
bool isSortedUntil(Iterator first, const Iterator last, Compare comp)
{
	if (first == last) { return first; }

	Iterator next = first;
	while (++next != last)
	{
		if (comp(*next, *first))
		{
			return next;
		}
		++first;
	}
	return last;
}

/// Partition function for quicksort with iterators
template <class Iterator, class UnaryPredicate>
Iterator partition(Iterator first, Iterator last, UnaryPredicate pred)
{
	while (first != last)
	{
		while (pred(*first))
		{
			++first;
			if (first == last) { return first; }
		}

		do
		{
			--last;
			if (first == last) { return first; }
		} while (!pred(*last));
		swap(*first, *last);
		++first;
	}
	return first;
}

namespace {

/// Quicksort implementation with random access iterators, ascending order
template <class Iterator>
void quicksort(Iterator first, Iterator last, RandomAccessIteratorTag)
{
	int size = distance(first, last);
	if (size > 1)
	{
		Iterator p = prev(last);
		swap(*next(first, size / 2), *p);
		Iterator q = partition(first, p, IsLessThan<typename IteratorTraits<Iterator>::ValueType>(*p));
		swap(*q, *p);
		quicksort(first, q, RandomAccessIteratorTag());
		quicksort(next(q), last, RandomAccessIteratorTag());
	}
}

/// Quicksort implementation with random access iterators, descending order
template <class Iterator>
void quicksortDesc(Iterator first, Iterator last, RandomAccessIteratorTag)
{
	int size = distance(first, last);
	if (size > 1)
	{
		Iterator p = prev(last);
		swap(*next(first, size / 2), *p);
		Iterator q = partition(first, p, IsNotLessThan<typename IteratorTraits<Iterator>::ValueType>(*p));
		swap(*q, *p);
		quicksortDesc(first, q, RandomAccessIteratorTag());
		quicksortDesc(next(q), last, RandomAccessIteratorTag());
	}
}

/// Quicksort implementation with bidirectional iterators, ascending order
template <class Iterator>
void quicksort(Iterator first, Iterator last, BidirectionalIteratorTag)
{
	if (first != last)
	{
		Iterator p = prev(last);
		swap(*first, *p);
		Iterator q = partition(first, p, IsLessThan<typename IteratorTraits<Iterator>::ValueType>(*p));
		swap(*q, *p);
		quicksort(first, q, BidirectionalIteratorTag());
		quicksort(next(q), last, BidirectionalIteratorTag());
	}
}

/// Quicksort implementation with bidirectional iterators, descending order
template <class Iterator>
void quicksortDesc(Iterator first, Iterator last, BidirectionalIteratorTag)
{
	if (first != last)
	{
		Iterator p = prev(last);
		swap(*first, *p);
		Iterator q = partition(first, p, IsNotLessThan<typename IteratorTraits<Iterator>::ValueType>(*p));
		swap(*q, *p);
		quicksortDesc(first, q, BidirectionalIteratorTag());
		quicksortDesc(next(q), last, BidirectionalIteratorTag());
	}
}

}

/// Quicksort implementation with iterators, ascending order
template <class Iterator>
void quicksort(Iterator first, Iterator last)
{
	quicksort(first, last, IteratorTraits<Iterator>::IteratorCategory());
}

/// Quicksort implementation with iterators, descending order
template <class Iterator>
void quicksortDesc(Iterator first, Iterator last)
{
	quicksortDesc(first, last, IteratorTraits<Iterator>::IteratorCategory());
}

}

namespace nc = ncine;

#endif
