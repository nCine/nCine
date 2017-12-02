#ifndef CLASS_NCINE_ARRAYITERATOR
#define CLASS_NCINE_ARRAYITERATOR

#include "iterator_traits.h"

namespace ncine {

/// An Array iterator
template <class T, bool IsConst>
class ArrayIterator
{
  public:
	/// Pointer type which respects iterator constness
	using Pointer = typename IteratorTraits<ArrayIterator>::Pointer;
	/// Reference type which respects iterator constness
	using Reference = typename IteratorTraits<ArrayIterator>::Reference;

	explicit ArrayIterator(T *elementPtr)
		: elementPtr_(elementPtr) { }

	/// Copy constructor to implicitly convert a non constant iterator to a constant one
	ArrayIterator(const ArrayIterator<T, false> &it)
		: elementPtr_(it.elementPtr_) { }

	/// Deferencing operator
	Reference operator*() const;

	/// Iterates to the next element (prefix)
	ArrayIterator &operator++();
	/// Iterates to the next element (postfix)
	ArrayIterator operator++(int);

	/// Iterates to the previous element (prefix)
	ArrayIterator &operator--();
	/// Iterates to the previous element (postfix)
	ArrayIterator operator--(int);

	/// Compound addition operator
	ArrayIterator &operator+=(int n);
	/// Compound subtraction operator
	ArrayIterator &operator-=(int n);
	/// Addition operator
	ArrayIterator operator+(int n) const;
	/// Subtraction operator
	ArrayIterator operator-(int n) const;
	/// Pointer subtraction operator
	friend inline int operator-(const ArrayIterator &lhs, const ArrayIterator &rhs) { return static_cast<int>(lhs.elementPtr_ - rhs.elementPtr_); }

	/// Subscript operator
	Reference operator[](int n) const;

	/// Equality operator
	friend inline bool operator==(const ArrayIterator &lhs, const ArrayIterator &rhs) { return lhs.elementPtr_ == rhs.elementPtr_; }

	/// Inequality operator
	friend inline bool operator!=(const ArrayIterator &lhs, const ArrayIterator &rhs) { return lhs.elementPtr_ != rhs.elementPtr_; }

	/// Greater than operator
	friend inline bool operator>(const ArrayIterator &lhs, const ArrayIterator &rhs) { return lhs.elementPtr_ > rhs.elementPtr_; }
	/// Less than operator
	friend inline bool operator<(const ArrayIterator &lhs, const ArrayIterator &rhs) { return lhs.elementPtr_ < rhs.elementPtr_; }
	/// Greater than or equal to operator
	friend inline bool operator>=(const ArrayIterator &lhs, const ArrayIterator &rhs) { return lhs.elementPtr_ >= rhs.elementPtr_; }
	/// Less than or equal to operator
	friend inline bool operator<=(const ArrayIterator &lhs, const ArrayIterator &rhs) { return lhs.elementPtr_ <= rhs.elementPtr_; }

  private:
	Pointer elementPtr_;

	/// For non constant to constant iterator implicit conversion
	friend class ArrayIterator<T, true>;
};

/// Iterator traits structure specialization for `ArrayIterator` class
template <class T>
struct IteratorTraits<ArrayIterator<T, false> >
{
	/// Type of the values deferenced by the iterator
	using ValueType = T;
	/// Pointer to the type of the values deferenced by the iterator
	using Pointer = T *;
	/// Reference to the type of the values deferenced by the iterator
	using Reference = T &;
	/// Type trait for iterator category
	static inline RandomAccessIteratorTag IteratorCategory() { return RandomAccessIteratorTag(); }
};

/// Iterator traits structure specialization for constant `ArrayIterator` class
template <class T>
struct IteratorTraits<ArrayIterator<T, true> >
{
	/// Type of the values deferenced by the iterator (never const)
	using ValueType = T;
	/// Pointer to the type of the values deferenced by the iterator
	using Pointer = const T *;
	/// Reference to the type of the values deferenced by the iterator
	using Reference = const T &;
	/// Type trait for iterator category
	static inline RandomAccessIteratorTag IteratorCategory() { return RandomAccessIteratorTag(); }
};

template <class T, bool IsConst>
inline typename ArrayIterator<T, IsConst>::Reference ArrayIterator<T, IsConst>::operator*() const
{
	ASSERT(elementPtr_);
	return *elementPtr_;
}

template <class T, bool IsConst>
ArrayIterator<T, IsConst> &ArrayIterator<T, IsConst>::operator++()
{
	++elementPtr_;

	return *this;
}

template <class T, bool IsConst>
ArrayIterator<T, IsConst> ArrayIterator<T, IsConst>::operator++(int)
{
	// Create an unmodified copy to return
	ArrayIterator<T, IsConst> iterator = *this;

	++elementPtr_;

	return iterator;
}

template <class T, bool IsConst>
ArrayIterator<T, IsConst> &ArrayIterator<T, IsConst>::operator--()
{
	--elementPtr_;

	return *this;
}

template <class T, bool IsConst>
ArrayIterator<T, IsConst> ArrayIterator<T, IsConst>::operator--(int)
{
	// Create an unmodified copy to return
	ArrayIterator<T, IsConst> iterator = *this;

	--elementPtr_;

	return iterator;
}

template <class T, bool IsConst>
ArrayIterator<T, IsConst> &ArrayIterator<T, IsConst>::operator+=(int n)
{
	elementPtr_ += n;

	return *this;
}

template <class T, bool IsConst>
ArrayIterator<T, IsConst> &ArrayIterator<T, IsConst>::operator-=(int n)
{
	elementPtr_ -= n;

	return *this;
}

template <class T, bool IsConst>
ArrayIterator<T, IsConst> ArrayIterator<T, IsConst>::operator+(int n) const
{
	ArrayIterator<T, IsConst> iterator = *this;
	iterator.elementPtr_ += n;

	return iterator;
}

template <class T, bool IsConst>
ArrayIterator<T, IsConst> ArrayIterator<T, IsConst>::operator-(int n) const
{
	ArrayIterator<T, IsConst> iterator = *this;
	iterator.elementPtr_ -= n;

	return iterator;
}

template <class T, bool IsConst>
inline typename ArrayIterator<T, IsConst>::Reference ArrayIterator<T, IsConst>::operator[](int n) const
{
	return *(elementPtr_ + n);
}

}

#endif
