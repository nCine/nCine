#ifndef CLASS_NCINE_STRINGITERATOR
#define CLASS_NCINE_STRINGITERATOR

#include "common_defines.h"
#include "common_macros.h"
#include "iterator_traits.h"

namespace ncine {

template <bool IsConst> class StringIterator;

/// Iterator traits structure specialization for `StringIterator` class
template < >
struct IteratorTraits<StringIterator<false> >
{
	/// Type of the values deferenced by the iterator
	typedef char ValueType;
	/// Pointer to the type of the values deferenced by the iterator
	typedef char *Pointer;
	/// Reference to the type of the values deferenced by the iterator
	typedef char &Reference;
	/// Type trait for iterator category
	static inline RandomAccessIteratorTag IteratorCategory() { return RandomAccessIteratorTag(); }
};

/// Iterator traits structure specialization for constant `StringIterator` class
template < >
struct IteratorTraits<StringIterator<true> >
{
	/// Type of the values deferenced by the iterator (never const)
	typedef char ValueType;
	/// Pointer to the type of the values deferenced by the iterator
	typedef const char *Pointer;
	/// Reference to the type of the values deferenced by the iterator
	typedef const char &Reference;
	/// Type trait for iterator category
	static inline RandomAccessIteratorTag IteratorCategory() { return RandomAccessIteratorTag(); }
};

/// A String iterator
template <bool IsConst>
class StringIterator
{
  public:
	/// Pointer type which respects iterator constness
	typedef typename IteratorTraits<StringIterator>::Pointer Pointer;
	/// Reference type which respects iterator constness
	typedef typename IteratorTraits<StringIterator>::Reference Reference;

	explicit StringIterator(char *c)
		: charPtr_(c) { }

	/// Copy constructor to implicitly convert a non constant iterator to a constant one
	StringIterator(const StringIterator<false> &it)
		: charPtr_(it.charPtr_) { }

	/// Deferencing operator
	Reference operator*() const;

	/// Iterates to the next element (prefix)
	StringIterator &operator++();
	/// Iterates to the next element (postfix)
	StringIterator operator++(int);

	/// Iterates to the previous element (prefix)
	StringIterator &operator--();
	/// Iterates to the previous element (postfix)
	StringIterator operator--(int);

	/// Compound addition operator
	StringIterator &operator+=(int n);
	/// Compound subtraction operator
	StringIterator &operator-=(int n);
	/// Addition operator
	StringIterator operator+(int n) const;
	/// Subtraction operator
	StringIterator operator-(int n) const;
	/// Pointer subtraction operator
	friend inline int operator-(const StringIterator &lhs, const StringIterator &rhs)	{ return (lhs.charPtr_ - rhs.charPtr_); }

	/// Subscript operator
	Reference operator[](int n) const;

	/// Equality operator
	friend inline bool operator==(const StringIterator &lhs, const StringIterator &rhs) { return lhs.charPtr_ == rhs.charPtr_; }
	/// Inequality operator
	friend inline bool operator!=(const StringIterator &lhs, const StringIterator &rhs) { return lhs.charPtr_ != rhs.charPtr_; }

	/// Greater than operator
	friend inline bool operator>(const StringIterator &lhs, const StringIterator &rhs) { return lhs.charPtr_ > rhs.charPtr_; }
	/// Less than operator
	friend inline bool operator<(const StringIterator &lhs, const StringIterator &rhs) { return lhs.charPtr_ < rhs.charPtr_; }
	/// Greater than or equal to operator
	friend inline bool operator>=(const StringIterator &lhs, const StringIterator &rhs) { return lhs.charPtr_ >= rhs.charPtr_; }
	/// Less than or equal to operator
	friend inline bool operator<=(const StringIterator &lhs, const StringIterator &rhs) { return lhs.charPtr_ <= rhs.charPtr_; }

  private:
	Pointer charPtr_;

	/// For non constant to constant iterator implicit conversion
	friend class StringIterator<true>;
};

template <bool IsConst>
inline typename StringIterator<IsConst>::Reference StringIterator<IsConst>::operator*() const
{
	ASSERT(charPtr_);
	return *charPtr_;
}

template <bool IsConst>
inline StringIterator<IsConst> &StringIterator<IsConst>::operator++()
{
	++charPtr_;

	return *this;
}

template <bool IsConst>
inline StringIterator<IsConst> StringIterator<IsConst>::operator++(int)
{
	// Create an unmodified copy to return
	StringIterator iterator = *this;

	++charPtr_;

	return iterator;
}

template <bool IsConst>
inline StringIterator<IsConst> &StringIterator<IsConst>::operator--()
{
	--charPtr_;

	return *this;
}

template <bool IsConst>
inline StringIterator<IsConst> StringIterator<IsConst>::operator--(int)
{
	// Create an unmodified copy to return
	StringIterator iterator = *this;

	--charPtr_;

	return iterator;
}

template <bool IsConst>
inline StringIterator<IsConst> &StringIterator<IsConst>::operator+=(int n)
{
	charPtr_ += n;

	return *this;
}

template <bool IsConst>
inline StringIterator<IsConst> &StringIterator<IsConst>::operator-=(int n)
{
	charPtr_ -= n;

	return *this;
}

template <bool IsConst>
inline StringIterator<IsConst> StringIterator<IsConst>::operator+(int n) const
{
	StringIterator iterator = *this;
	iterator.charPtr_ += n;

	return iterator;
}

template <bool IsConst>
inline StringIterator<IsConst> StringIterator<IsConst>::operator-(int n) const
{
	StringIterator iterator = *this;
	iterator.charPtr_ -= n;

	return iterator;
}

template <bool IsConst>
inline typename StringIterator<IsConst>::Reference StringIterator<IsConst>::operator[](int n) const
{
	return *(charPtr_ + n);
}

}

#endif
