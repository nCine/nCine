#ifndef CLASS_NCTL_LISTITERATOR
#define CLASS_NCTL_LISTITERATOR

#include <ncine/common_macros.h>
#include "iterator.h"

namespace nctl {

template <class T> class List;
template <class T> class ListNode;

/// A List iterator
template <class T, bool IsConst>
class ListIterator
{
  public:
	/// Reference type which respects iterator constness
	using Reference = typename IteratorTraits<ListIterator>::Reference;

	explicit ListIterator(ListNode<T> *node)
		: node_(node) { }

	/// Copy constructor to implicitly convert a non constant iterator to a constant one
	ListIterator(const ListIterator<T, false> &it)
		: node_(it.node_) { }

	/// Deferencing operator
	Reference operator*() const;

	/// Iterates to the next element (prefix)
	ListIterator &operator++();
	/// Iterates to the next element (postfix)
	ListIterator operator++(int);

	/// Iterates to the previous element (prefix)
	ListIterator &operator--();
	/// Iterates to the previous element (postfix)
	ListIterator operator--(int);

	/// Equality operator
	friend inline bool operator==(const ListIterator &lhs, const ListIterator &rhs) { return lhs.node_ == rhs.node_; }
	/// Inequality operator
	friend inline bool operator!=(const ListIterator &lhs, const ListIterator &rhs) { return lhs.node_ != rhs.node_; }

  private:
	ListNode<T> *node_;

	friend class List<T>;

	/// For non constant to constant iterator implicit conversion
	friend class ListIterator<T, true>;
};

/// Iterator traits structure specialization for `ListIterator` class
template <class T>
struct IteratorTraits<ListIterator<T, false> >
{
	/// Type of the values deferenced by the iterator
	using ValueType = T;
	/// Pointer to the type of the values deferenced by the iterator
	using Pointer = T *;
	/// Reference to the type of the values deferenced by the iterator
	using Reference = T &;
	/// Type trait for iterator category
	static inline BidirectionalIteratorTag IteratorCategory() { return BidirectionalIteratorTag(); }
};

/// Iterator traits structure specialization for constant `ListIterator` class
template <class T>
struct IteratorTraits<ListIterator<T, true> >
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

template <class T, bool IsConst>
inline typename ListIterator<T, IsConst>::Reference ListIterator<T, IsConst>::operator*() const
{
	return node_->data_;
}

template <class T, bool IsConst>
ListIterator<T, IsConst> &ListIterator<T, IsConst>::operator++()
{
	node_ = static_cast<ListNode<T> *>(node_->next_);
	return *this;
}

template <class T, bool IsConst>
ListIterator<T, IsConst> ListIterator<T, IsConst>::operator++(int)
{
	// Create an unmodified copy to return
	ListIterator iterator = *this;

	node_ = static_cast<ListNode<T> *>(node_->next_);

	return iterator;
}

template <class T, bool IsConst>
ListIterator<T, IsConst> &ListIterator<T, IsConst>::operator--()
{
	node_ = static_cast<ListNode<T> *>(node_->previous_);
	return *this;
}

template <class T, bool IsConst>
ListIterator<T, IsConst> ListIterator<T, IsConst>::operator--(int)
{
	// Create an unmodified copy to return
	ListIterator iterator = *this;

	node_ = static_cast<ListNode<T> *>(node_->previous_);

	return iterator;
}

}

#endif
