#ifndef NCTL_ITERATOR
#define NCTL_ITERATOR

namespace nctl {

/// Dispatching tag for iterators that can only move forward, one element at a time
struct ForwardIteratorTag
{};
/// Dispatching tag for iterators that can move both ways, one element at a time
struct BidirectionalIteratorTag : public ForwardIteratorTag
{};
/// Dispatching tag for iterators that can jump arbitrary distances in both ways
struct RandomAccessIteratorTag : public BidirectionalIteratorTag
{};

/// Base iterator traits structure
template <class Iterator>
struct IteratorTraits
{};

///////////////////////////////////////////////////////////
// OPERATIONS
///////////////////////////////////////////////////////////

namespace {

	/// Increments an iterator by n elements, for random access iterators
	template <class Iterator>
	inline void advance(Iterator &it, int n, RandomAccessIteratorTag)
	{
		it += n;
	}

	/// Increments an iterator by n elements, for bidirectional iterators
	template <class Iterator>
	inline void advance(Iterator &it, int n, BidirectionalIteratorTag)
	{
		if (n < 0)
		{
			while (n++)
				--it;
		}
		else
		{
			while (n--)
				++it;
		}
	}

	/// Increments an iterator by n elements, for forward iterators
	template <class Iterator>
	inline void advance(Iterator &it, int n, ForwardIteratorTag)
	{
		if (n > 0)
		{
			while (n--)
				++it;
		}
	}

}

/// Increments an iterator by n elements
template <class Iterator>
inline void advance(Iterator &it, int n)
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
	inline int distance(RandomAccessIterator &first, const RandomAccessIterator &last, RandomAccessIteratorTag)
	{
		return last - first;
	}

	/// Returns the distance in number of increments between two forward iterators
	template <class ForwardIterator>
	inline int distance(ForwardIterator &first, const ForwardIterator &last, ForwardIteratorTag)
	{
		int counter = 0;

		for (; first != last; ++first)
			counter++;

		return counter;
	}

}

/// Returns the distance between two iterators
template <class Iterator>
inline int distance(Iterator first, const Iterator last)
{
	return distance(first, last, IteratorTraits<Iterator>::IteratorCategory());
}

///////////////////////////////////////////////////////////
// REVERSE RANGE ADAPTER
///////////////////////////////////////////////////////////

template <class T>
struct ReversionWrapper
{
	T &iterable;
};

template <class T>
auto begin(ReversionWrapper<T> c) -> decltype(rBegin(c.iterable))
{
	return rBegin(c.iterable);
}

template <class T>
auto end(ReversionWrapper<T> c) -> decltype(rEnd(c.iterable))
{
	return rEnd(c.iterable);
}

template <class T>
ReversionWrapper<T> reverse(T &&iterable)
{
	return { iterable };
}

///////////////////////////////////////////////////////////
// RANGE
///////////////////////////////////////////////////////////

template <class Container>
typename Container::Iterator begin(Container &c)
{
	return c.begin();
}

template <class Container>
typename Container::ConstIterator cBegin(const Container &c)
{
	return c.begin();
}

template <class Container>
typename Container::Iterator end(Container &c)
{
	return c.end();
}

template <class Container>
typename Container::ConstIterator cEnd(const Container &c)
{
	return c.end();
}

template <class Container>
typename Container::ReverseIterator rBegin(Container &c)
{
	return c.rBegin();
}

template <class Container>
typename Container::ConstReverseIterator crBegin(const Container &c)
{
	return c.rBegin();
}

template <class Container>
typename Container::ReverseIterator rEnd(Container &c)
{
	return c.rEnd();
}

template <class Container>
typename Container::ConstReverseIterator crEnd(const Container &c)
{
	return c.rEnd();
}

}

#endif
