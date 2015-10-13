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
	typedef typename IteratorTraits<ArrayIterator>::Pointer Pointer;
	/// Reference type which respects iterator constness
	typedef typename IteratorTraits<ArrayIterator>::Reference Reference;

	explicit ArrayIterator(T *elementPtr)
		: elementPtr_(elementPtr) { }

	/// Copy constructor to implicitly convert a non constant iterator to a constant one
	ArrayIterator(const ArrayIterator<T, false>& it)
		: elementPtr_(it.elementPtr_) { }

	// Deferencing operator
	Reference operator*() const;

	// Iterates to the next element (prefix)
	ArrayIterator& operator++();
	// Iterates to the next element (postfix)
	ArrayIterator operator++(int);

	// Iterates to the previous element (prefix)
	ArrayIterator& operator--();
	// Iterates to the previous element (postfix)
	ArrayIterator operator--(int);

	// Compound addition operator
	ArrayIterator& operator+=(int n);
	// Compound subtraction operator
	ArrayIterator& operator-=(int n);
	// Addition operator
	ArrayIterator operator+(int n) const;
	// Subtraction operator
	ArrayIterator operator-(int n) const;
	// Pointer subtraction operator
	int operator-(const ArrayIterator& iterator) const;

	// Subscript operator
	Reference operator[](int n) const;

	/// Equality operator
	inline bool operator==(const ArrayIterator& iterator) const { return elementPtr_ == iterator.elementPtr_; }
	/// Inequality operator
	inline bool operator!=(const ArrayIterator& iterator) const { return elementPtr_ != iterator.elementPtr_; }

	/// Greater than operator
	inline bool operator>(const ArrayIterator& iterator) const { return elementPtr_ > iterator.elementPtr_; }
	/// Less than operator
	inline bool operator<(const ArrayIterator& iterator) const { return elementPtr_ < iterator.elementPtr_; }
	/// Greater than or equal to operator
	inline bool operator>=(const ArrayIterator& iterator) const { return elementPtr_ >= iterator.elementPtr_; }
	/// Less than or equal to operator
	inline bool operator<=(const ArrayIterator& iterator) const { return elementPtr_ <= iterator.elementPtr_; }

  private:
	Pointer elementPtr_;

	// For non constant to constant iterator implicit conversion
	friend class ArrayIterator<T, true>;
};

/// Iterator traits structure specialization
template <class T>
struct IteratorTraits<ArrayIterator<T, false> >
{
	/// Type of the values deferenced by the iterator
	typedef T ValueType;
	/// Pointer to the type of the values deferenced by the iterator
	typedef T* Pointer;
	/// Reference to the type of the values deferenced by the iterator
	typedef T& Reference;
	/// Type trait for iterator category
	static inline RandomAccessIteratorTag IteratorCategory() { return RandomAccessIteratorTag(); }
};

/// Constant iterator traits structure specialization
template <class T>
struct IteratorTraits<ArrayIterator<T, true> >
{
	/// Type of the values deferenced by the iterator (never const)
	typedef T ValueType;
	/// Pointer to the type of the values deferenced by the iterator
	typedef const T* Pointer;
	/// Reference to the type of the values deferenced by the iterator
	typedef const T& Reference;
	/// Type trait for iterator category
	static inline RandomAccessIteratorTag IteratorCategory() { return RandomAccessIteratorTag(); }
};

/// Deferencing operator
template <class T, bool IsConst>
inline typename ArrayIterator<T, IsConst>::Reference ArrayIterator<T, IsConst>::operator*() const
{
	return *elementPtr_;
}

/// Iterates to the next element (prefix)
template <class T, bool IsConst>
ArrayIterator<T, IsConst>& ArrayIterator<T, IsConst>::operator++()
{
	++elementPtr_;

	return *this;
}

/// Iterates to the next element (postfix)
template <class T, bool IsConst>
ArrayIterator<T, IsConst> ArrayIterator<T, IsConst>::operator++(int)
{
	// Create an unmodified copy to return
	ArrayIterator<T, IsConst> iterator = *this;

	++elementPtr_;

	return iterator;
}

/// Iterates to the previous element (prefix)
template <class T, bool IsConst>
ArrayIterator<T, IsConst>& ArrayIterator<T, IsConst>::operator--()
{
	--elementPtr_;

	return *this;
}

/// Iterates to the previous element (postfix)
template <class T, bool IsConst>
ArrayIterator<T, IsConst> ArrayIterator<T, IsConst>::operator--(int)
{
	// Create an unmodified copy to return
	ArrayIterator<T, IsConst> iterator = *this;

	--elementPtr_;

	return iterator;
}

/// Compound addition operator
template <class T, bool IsConst>
ArrayIterator<T, IsConst>& ArrayIterator<T, IsConst>::operator+=(int n)
{
	elementPtr_ += n;

	return *this;
}

/// Compound subtraction operator
template <class T, bool IsConst>
ArrayIterator<T, IsConst>& ArrayIterator<T, IsConst>::operator-=(int n)
{
	elementPtr_ -= n;

	return *this;
}

/// Addition operator
template <class T, bool IsConst>
ArrayIterator<T, IsConst> ArrayIterator<T, IsConst>::operator+(int n) const
{
	ArrayIterator<T, IsConst> iterator = *this;
	iterator.elementPtr_ += n;

	return iterator;
}

/// Subtraction operator
template <class T, bool IsConst>
ArrayIterator<T, IsConst> ArrayIterator<T, IsConst>::operator-(int n) const
{
	ArrayIterator<T, IsConst> iterator = *this;
	iterator.elementPtr_ -= n;

	return iterator;
}

/// Pointer subtraction operator
template <class T, bool IsConst>
inline int ArrayIterator<T, IsConst>::operator-(const ArrayIterator& iterator) const
{
	return static_cast<int>(elementPtr_ - iterator.elementPtr_);
}

/// Subscript operator
template <class T, bool IsConst>
inline typename ArrayIterator<T, IsConst>::Reference ArrayIterator<T, IsConst>::operator[](int n) const
{
	return *(elementPtr_ + n);
}

}

#endif
