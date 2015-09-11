#ifndef CLASS_NCINE_STRINGITERATOR
#define CLASS_NCINE_STRINGITERATOR

#include "common_defines.h"
#include "iterator_traits.h"

namespace ncine {

template <bool IsConst> class StringIterator;

/// Iterator traits structure specialization
template < >
struct IteratorTraits<StringIterator<false> >
{
	/// Type of the values deferenced by the iterator
	typedef char ValueType;
	/// Pointer to the type of the values deferenced by the iterator
	typedef char* Pointer;
	/// Reference to the type of the values deferenced by the iterator
	typedef char& Reference;
	/// Type trait for iterator category
	static inline RandomAccessIteratorTag IteratorCategory() { return RandomAccessIteratorTag(); }
};

/// Constant iterator traits structure specialization
template < >
struct IteratorTraits<StringIterator<true> >
{
	/// Type of the values deferenced by the iterator (never const)
	typedef char ValueType;
	/// Pointer to the type of the values deferenced by the iterator
	typedef const char* Pointer;
	/// Reference to the type of the values deferenced by the iterator
	typedef const char& Reference;
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
	StringIterator(const StringIterator<false>& it)
		: charPtr_(it.charPtr_) { }

	// Deferencing operator
	Reference operator*() const;

	// Iterates to the next element (prefix)
	StringIterator& operator++();
	// Iterates to the next element (postfix)
	StringIterator operator++(int);

	// Iterates to the previous element (prefix)
	StringIterator& operator--();
	// Iterates to the previous element (postfix)
	StringIterator operator--(int);

	// Compound addition operator
	StringIterator& operator+=(int n);
	// Compound subtraction operator
	StringIterator& operator-=(int n);
	// Addition operator
	StringIterator operator+(int n) const;
	// Subtraction operator
	StringIterator operator-(int n) const;
	// Pointer subtraction operator
	int operator-(const StringIterator& iterator) const;

	// Subscript operator
	Reference operator[](int n) const;

	/// Equality operator
	inline bool operator==(const StringIterator& iterator) const { return charPtr_ == iterator.charPtr_; }
	/// Inequality operator
	inline bool operator!=(const StringIterator& iterator) const { return charPtr_ != iterator.charPtr_; }

	/// Greater than operator
	inline bool operator>(const StringIterator& iterator) const { return charPtr_ > iterator.charPtr_; }
	/// Less than operator
	inline bool operator<(const StringIterator& iterator) const { return charPtr_ < iterator.charPtr_; }
	/// Greater than or equal to operator
	inline bool operator>=(const StringIterator& iterator) const { return charPtr_ >= iterator.charPtr_; }
	/// Less than or equal to operator
	inline bool operator<=(const StringIterator& iterator) const { return charPtr_ <= iterator.charPtr_; }

  private:
	Pointer charPtr_;

	// For non constant to constant iterator implicit conversion
	friend class StringIterator<true>;
};

/// Deferencing operator
template <bool IsConst>
inline typename StringIterator<IsConst>::Reference StringIterator<IsConst>::operator*() const
{
	return *charPtr_;
}

/// Iterates to the next element (prefix)
template <bool IsConst>
inline StringIterator<IsConst>& StringIterator<IsConst>::operator++()
{
	++charPtr_;

	return *this;
}

/// Iterates to the next element (postfix)
template <bool IsConst>
inline StringIterator<IsConst> StringIterator<IsConst>::operator++(int)
{
	// Create an unmodified copy to return
	StringIterator iterator = *this;

	++charPtr_;

	return iterator;
}

/// Iterates to the previous element (prefix)
template <bool IsConst>
inline StringIterator<IsConst>& StringIterator<IsConst>::operator--()
{
	--charPtr_;

	return *this;
}

/// Iterates to the previous element (postfix)
template <bool IsConst>
inline StringIterator<IsConst> StringIterator<IsConst>::operator--(int)
{
	// Create an unmodified copy to return
	StringIterator iterator = *this;

	--charPtr_;

	return iterator;
}

/// Compound addition operator
template <bool IsConst>
inline StringIterator<IsConst>& StringIterator<IsConst>::operator+=(int n)
{
	charPtr_ += n;

	return *this;
}

/// Compound subtraction operator
template <bool IsConst>
inline StringIterator<IsConst>& StringIterator<IsConst>::operator-=(int n)
{
	charPtr_ -= n;

	return *this;
}

/// Addition operator
template <bool IsConst>
inline StringIterator<IsConst> StringIterator<IsConst>::operator+(int n) const
{
	StringIterator iterator = *this;
	iterator.charPtr_ += n;

	return iterator;
}

/// Subtraction operator
template <bool IsConst>
inline StringIterator<IsConst> StringIterator<IsConst>::operator-(int n) const
{
	StringIterator iterator = *this;
	iterator.charPtr_ -= n;

	return iterator;
}

/// Pointer subtraction operator
template <bool IsConst>
inline int StringIterator<IsConst>::operator-(const StringIterator& iterator) const
{
	return (charPtr_ - iterator.charPtr_);
}

/// Subscript operator
template <bool IsConst>
inline typename StringIterator<IsConst>::Reference StringIterator<IsConst>::operator[](int n) const
{
	return *(charPtr_ + n);
}

}

#endif
