#ifndef CLASS_NCINE_LISTITERATOR
#define CLASS_NCINE_LISTITERATOR

#include "common_macros.h"
#include "iterator_traits.h"

namespace ncine {

template <class T> class List;
template <class T> class ListNode;

/// A List iterator
template <class T, bool IsConst>
class ListIterator
{
  public:
	/// Reference type which respects iterator constness
	typedef typename IteratorTraits<ListIterator>::Reference Reference;

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
	typedef T ValueType;
	/// Pointer to the type of the values deferenced by the iterator
	typedef T *Pointer;
	/// Reference to the type of the values deferenced by the iterator
	typedef T &Reference;
	/// Type trait for iterator category
	static inline BidirectionalIteratorTag IteratorCategory() { return BidirectionalIteratorTag(); }
};

/// Iterator traits structure specialization for constant `ListIterator` class
template <class T>
struct IteratorTraits<ListIterator<T, true> >
{
	/// Type of the values deferenced by the iterator (never const)
	typedef T ValueType;
	/// Pointer to the type of the values deferenced by the iterator
	typedef const T *Pointer;
	/// Reference to the type of the values deferenced by the iterator
	typedef const T &Reference;
	/// Type trait for iterator category
	static inline BidirectionalIteratorTag IteratorCategory() { return BidirectionalIteratorTag(); }
};

template <class T, bool IsConst>
inline typename ListIterator<T, IsConst>::Reference ListIterator<T, IsConst>::operator*() const
{
	ASSERT(node_);
	// Cannot simply return only if node_ is not a `nullptr` or
	// "control may reach end of non-void function"
	return node_->data_;
}

template <class T, bool IsConst>
ListIterator<T, IsConst> &ListIterator<T, IsConst>::operator++()
{
	if (node_)
		node_ = node_->next_;

	return *this;
}

template <class T, bool IsConst>
ListIterator<T, IsConst> ListIterator<T, IsConst>::operator++(int)
{
	// Create an unmodified copy to return
	ListIterator iterator = *this;

	if (node_)
		node_ = node_->next_;

	return iterator;
}

template <class T, bool IsConst>
ListIterator<T, IsConst> &ListIterator<T, IsConst>::operator--()
{
	if (node_)
		node_ = node_->previous_;

	return *this;
}

template <class T, bool IsConst>
ListIterator<T, IsConst> ListIterator<T, IsConst>::operator--(int)
{
	// Create an unmodified copy to return
	ListIterator iterator = *this;

	if (node_)
		node_ = node_->previous_;

	return iterator;
}

}

#endif
