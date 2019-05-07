#ifndef CLASS_NCTL_SPARSESETITERATOR
#define CLASS_NCTL_SPARSESETITERATOR

#include "SparseSet.h"
#include "iterator.h"

namespace nctl {

/// A sparseset iterator
template <class T>
class SparseSetIterator
{
  public:
	/// Reference type which respects iterator constness
	using Reference = typename IteratorTraits<SparseSetIterator>::Reference;

	SparseSetIterator(const SparseSet<T> *sparseSet, int index)
	    : sparseSet_(sparseSet), index_(index) {}

	/// Deferencing operator
	Reference operator*() const;

	/// Iterates to the next element (prefix)
	SparseSetIterator &operator++();
	/// Iterates to the next element (postfix)
	SparseSetIterator operator++(int);

	/// Iterates to the previous element (prefix)
	SparseSetIterator &operator--();
	/// Iterates to the previous element (postfix)
	SparseSetIterator operator--(int);

	/// Equality operator
	friend inline bool operator==(const SparseSetIterator &lhs, const SparseSetIterator &rhs)
	{
		return (lhs.sparseSet_ == rhs.sparseSet_ && lhs.index_ == rhs.index_);
	}

	/// Inequality operator
	friend inline bool operator!=(const SparseSetIterator &lhs, const SparseSetIterator &rhs)
	{
		return (lhs.sparseSet_ != rhs.sparseSet_ || lhs.index_ != rhs.index_);
	}

  private:
	const SparseSet<T> *sparseSet_;
	int index_;
};

/// Iterator traits structure specialization for `SparseSetIterator` class
template <class T>
struct IteratorTraits<SparseSetIterator<T>>
{
	/// Type of the values deferenced by the iterator (never const)
	using ValueType = T;
	/// Pointer to the type of the values deferenced by the iterator
	using Pointer = const T *;
	/// Reference to the type of the values deferenced by the iterator
	using Reference = const T &;
	/// Type trait for iterator category
	static inline BidirectionalIteratorTag IteratorCategory() { return BidirectionalIteratorTag(); }
};

template <class T>
typename SparseSetIterator<T>::Reference SparseSetIterator<T>::operator*() const
{
	return sparseSet_->dense_[sparseSet_->sparse_[index_]];
}

template <class T>
SparseSetIterator<T> &SparseSetIterator<T>::operator++()
{
	index_++;
	return *this;
}

template <class T>
SparseSetIterator<T> SparseSetIterator<T>::operator++(int)
{
	// Create an unmodified copy to return
	SparseSetIterator<T> iterator = *this;
	index_++;
	return iterator;
}

template <class T>
SparseSetIterator<T> &SparseSetIterator<T>::operator--()
{
	index_--;
	return *this;
}

template <class T>
SparseSetIterator<T> SparseSetIterator<T>::operator--(int)
{
	// Create an unmodified copy to return
	SparseSetIterator<T> iterator = *this;
	index_--;
	return iterator;
}

}

#endif
