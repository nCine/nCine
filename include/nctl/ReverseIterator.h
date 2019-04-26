#ifndef CLASS_NCTL_REVERSEITERATOR
#define CLASS_NCTL_REVERSEITERATOR

#include "iterator.h"

namespace nctl {

/// A generic reverse iterator
template <class Iterator>
class ReverseIterator
{
  public:
	/// Pointer type which respects iterator constness
	using Pointer = typename IteratorTraits<Iterator>::Pointer;
	/// Reference type which respects iterator constness
	using Reference = typename IteratorTraits<Iterator>::Reference;

	explicit ReverseIterator(Iterator it)
	    : it_(it) {}

	/// Converting constructor between compatible reverse iterators
	template <typename Iter>
	ReverseIterator(const ReverseIterator<Iter> &reverseIt)
	    : it_(reverseIt.base()) {}

	/// Deferencing operator
	Reference operator*() const;

	/// Iterates to the next element (reversed prefix)
	ReverseIterator &operator++();
	/// Iterates to the next element (reversed postfix)
	ReverseIterator operator++(int);

	/// Iterates to the previous element (reversed prefix)
	ReverseIterator &operator--();
	/// Iterates to the previous element (reversed postfix)
	ReverseIterator operator--(int);

	/// Compound addition operator (reversed)
	ReverseIterator &operator+=(int n);
	/// Compound subtraction operator (reversed)
	ReverseIterator &operator-=(int n);
	/// Addition operator (reversed)
	ReverseIterator operator+(int n) const;
	/// Subtraction operator (reversed)
	ReverseIterator operator-(int n) const;
	/// Pointer subtraction operator (reversed)
	friend inline int operator-(const ReverseIterator &lhs, const ReverseIterator &rhs) { return -static_cast<int>(lhs.it_ - rhs.it_); }
	/// Pointer subtraction operator for compatible reverse iterators (reversed)
	template <typename Iter>
	friend inline int operator-(const ReverseIterator &lhs, const ReverseIterator<Iter> &rhs) { return -static_cast<int>(lhs.it_ - rhs.base()); }

	/// Subscript operator
	Reference operator[](int n) const;

	/// Equality operator
	friend inline bool operator==(const ReverseIterator &lhs, const ReverseIterator &rhs) { return lhs.it_ == rhs.it_; }
	/// Equality operator for compatible reverse iterators
	template <typename Iter>
	friend inline bool operator==(const ReverseIterator &lhs, const ReverseIterator<Iter> &rhs) { return lhs.it_ == rhs.base(); }

	/// Inequality operator
	friend inline bool operator!=(const ReverseIterator &lhs, const ReverseIterator &rhs) { return lhs.it_ != rhs.it_; }
	/// Inequality operator for compatible reverse iterators
	template <typename Iter>
	friend inline bool operator!=(const ReverseIterator &lhs, const ReverseIterator<Iter> &rhs) { return lhs.it_ != rhs.base(); }

	/// Greater than operator (reversed)
	friend inline bool operator>(const ReverseIterator &lhs, const ReverseIterator &rhs) { return lhs.it_ < rhs.it_; }
	template <typename Iter>
	/// Greater than operator for compatible reverse iterators (reversed)
	friend inline bool operator>(const ReverseIterator &lhs, const ReverseIterator<Iter> &rhs) { return lhs.it_ < rhs.base(); }
	/// Less than operator (reversed)
	friend inline bool operator<(const ReverseIterator &lhs, const ReverseIterator &rhs) { return lhs.it_ > rhs.it_; }
	/// Less than operator for compatible reverse iterators (reversed)
	template <typename Iter>
	friend inline bool operator<(const ReverseIterator &lhs, const ReverseIterator<Iter> &rhs) { return lhs.it_ > rhs.base(); }
	/// Greater than or equal to operator (reversed)
	friend inline bool operator>=(const ReverseIterator &lhs, const ReverseIterator &rhs) { return lhs.it_ <= rhs.it_; }
	/// Greater than or equal to operator for compatible reverse iterators (reversed)
	template <typename Iter>
	friend inline bool operator>=(const ReverseIterator &lhs, const ReverseIterator<Iter> &rhs) { return lhs.it_ <= rhs.base(); }
	/// Less than or equal to operator (reversed)
	friend inline bool operator<=(const ReverseIterator &lhs, const ReverseIterator &rhs) { return lhs.it_ >= rhs.it_; }
	/// Less than or equal to operator for compatible reverse iterators (reversed)
	template <typename Iter>
	friend inline bool operator<=(const ReverseIterator &lhs, const ReverseIterator<Iter> &rhs) { return lhs.it_ >= rhs.base(); }

	/// Returns the base iterator
	Iterator base() const;

  private:
	Iterator it_;
};

/// Iterator traits structure specialization for `ReverseIterator` class
template <class Iterator>
struct IteratorTraits<ReverseIterator<Iterator>>
{
	/// Type of the values deferenced by the iterator
	using ValueType = typename Iterator::ValueType;
	/// Pointer to the type of the values deferenced by the iterator
	using Pointer = typename Iterator::Pointer;
	/// Reference to the type of the values deferenced by the iterator
	using Reference = typename Iterator::Reference;
	/// Type trait for iterator category
	static inline decltype(Iterator::IteratorCategory()) IteratorCategory() { return Iterator::IteratorCategory(); }
};

template <class Iterator>
inline typename ReverseIterator<Iterator>::Reference ReverseIterator<Iterator>::operator*() const
{
	return *it_;
}

template <class Iterator>
ReverseIterator<Iterator> &ReverseIterator<Iterator>::operator++()
{
	--it_;

	return *this;
}

template <class Iterator>
ReverseIterator<Iterator> ReverseIterator<Iterator>::operator++(int)
{
	// Create an unmodified copy to return
	ReverseIterator<Iterator> iterator = *this;

	--it_;

	return iterator;
}

template <class Iterator>
ReverseIterator<Iterator> &ReverseIterator<Iterator>::operator--()
{
	++it_;

	return *this;
}

template <class Iterator>
ReverseIterator<Iterator> ReverseIterator<Iterator>::operator--(int)
{
	// Create an unmodified copy to return
	ReverseIterator<Iterator> iterator = *this;

	++it_;

	return iterator;
}

template <class Iterator>
ReverseIterator<Iterator> &ReverseIterator<Iterator>::operator+=(int n)
{
	it_ -= n;

	return *this;
}

template <class Iterator>
ReverseIterator<Iterator> &ReverseIterator<Iterator>::operator-=(int n)
{
	it_ += n;

	return *this;
}

template <class Iterator>
ReverseIterator<Iterator> ReverseIterator<Iterator>::operator+(int n) const
{
	ReverseIterator<Iterator> iterator = *this;
	iterator.it_ -= n;

	return iterator;
}

template <class Iterator>
ReverseIterator<Iterator> ReverseIterator<Iterator>::operator-(int n) const
{
	ReverseIterator<Iterator> iterator = *this;
	iterator.it_ += n;

	return iterator;
}

template <class Iterator>
inline typename ReverseIterator<Iterator>::Reference ReverseIterator<Iterator>::operator[](int n) const
{
	return *(it_ - n);
}

template <class Iterator>
inline Iterator ReverseIterator<Iterator>::base() const
{
	return it_;
}

}

#endif
