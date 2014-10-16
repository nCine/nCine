#ifndef CLASS_NCLISTITERATOR
#define CLASS_NCLISTITERATOR

template <class T> class ncList; // forward declaration
template <class T> class ncListNode; // forward declaration

/// A List iterator
template <class T>
class ncListIterator
{
  public:
	ncListIterator(ncListNode<T> *node)
		: node_(node) { }
	ncListIterator(const ncListIterator& iterator)
		: node_(iterator.node_) { }

	// Read-only deferencing operator
	inline const T& operator*() const;
	// Deferencing operator
	inline T& operator*();

	// Iterates to the next element (prefix)
	ncListIterator<T> operator++() const;
	// Iterates to the next element (postfix)
	ncListIterator<T> operator++(int) const;

	// Iterates to the previous element (prefix)
	ncListIterator<T> operator--() const;
	// Iterates to the previous element (postfix)
	ncListIterator<T> operator--(int) const;

	/// Equality operator
	inline bool operator==(const ncListIterator<T>& iterator) const { return node_ == iterator.node_; }
	/// Inequality operator
	inline bool operator!=(const ncListIterator<T>& iterator) const { return node_ != iterator.node_; }

  private:
	mutable ncListNode<T> *node_;

	friend class ncList<T>;
};

/// Read-only deferencing operator
template <class T>
const T& ncListIterator<T>::operator*() const
{
	// Cannot simply return only if node_ is not NULL or
	// "control may reach end of non-void function"
	return node_->data_;
}

/// Deferencing operator
template <class T>
T& ncListIterator<T>::operator*()
{
	// Cannot simply return only if node_ is not NULL or
	// "control may reach end of non-void function"
	return node_->data_;
}

/// Iterates to the next element (prefix)
template <class T>
ncListIterator<T> ncListIterator<T>::operator++() const
{
	if (node_)
	{
		node_ = node_->next_;
	}

	return *this;
}

/// Iterates to the next element (postfix)
template <class T>
ncListIterator<T> ncListIterator<T>::operator++(int) const
{
	// Create an unmodified copy to return
	ncListIterator<T> iterator = *this;

	if (node_)
	{
		node_ = node_->next_;
	}

	return iterator;
}

/// Iterates to the previous element (prefix)
template <class T>
ncListIterator<T> ncListIterator<T>::operator--() const
{
	if (node_)
	{
		node_ = node_->previous_;
	}

	return *this;
}

/// Iterates to the previous element (postfix)
template <class T>
ncListIterator<T> ncListIterator<T>::operator--(int) const
{
	// Create an unmodified copy to return
	ncListIterator<T> iterator = *this;

	if (node_)
	{
		node_ = node_->previous_;
	}

	return iterator;
}

#endif
