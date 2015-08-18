#ifndef CLASS_NCINE_LISTITERATOR
#define CLASS_NCINE_LISTITERATOR

namespace ncine {

template <class T> class List;
template <class T> class ListNode;

/// A List iterator
template <class T>
class ListIterator
{
  public:
	explicit ListIterator(ListNode<T> *node)
		: node_(node) { }
	ListIterator(const ListIterator& iterator)
		: node_(iterator.node_) { }

	// Read-only deferencing operator
	const T& operator*() const;
	// Deferencing operator
	T& operator*();

	// Iterates to the next element (prefix)
	ListIterator<T> operator++() const;
	// Iterates to the next element (postfix)
	ListIterator<T> operator++(int) const;

	// Iterates to the previous element (prefix)
	ListIterator<T> operator--() const;
	// Iterates to the previous element (postfix)
	ListIterator<T> operator--(int) const;

	/// Equality operator
	inline bool operator==(const ListIterator<T>& iterator) const { return node_ == iterator.node_; }
	/// Inequality operator
	inline bool operator!=(const ListIterator<T>& iterator) const { return node_ != iterator.node_; }

  private:
	mutable ListNode<T> *node_;

	friend class List<T>;
};

/// Read-only deferencing operator
template <class T>
inline const T& ListIterator<T>::operator*() const
{
	// Cannot simply return only if node_ is not NULL or
	// "control may reach end of non-void function"
	return node_->data_;
}

/// Deferencing operator
template <class T>
inline T& ListIterator<T>::operator*()
{
	// Cannot simply return only if node_ is not NULL or
	// "control may reach end of non-void function"
	return node_->data_;
}

/// Iterates to the next element (prefix)
template <class T>
ListIterator<T> ListIterator<T>::operator++() const
{
	if (node_)
	{
		node_ = node_->next_;
	}

	return *this;
}

/// Iterates to the next element (postfix)
template <class T>
ListIterator<T> ListIterator<T>::operator++(int) const
{
	// Create an unmodified copy to return
	ListIterator<T> iterator = *this;

	if (node_)
	{
		node_ = node_->next_;
	}

	return iterator;
}

/// Iterates to the previous element (prefix)
template <class T>
ListIterator<T> ListIterator<T>::operator--() const
{
	if (node_)
	{
		node_ = node_->previous_;
	}

	return *this;
}

/// Iterates to the previous element (postfix)
template <class T>
ListIterator<T> ListIterator<T>::operator--(int) const
{
	// Create an unmodified copy to return
	ListIterator<T> iterator = *this;

	if (node_)
	{
		node_ = node_->previous_;
	}

	return iterator;
}

}

#endif
