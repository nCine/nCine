#ifndef CLASS_NCLISTITERATOR
#define CLASS_NCLISTITERATOR

template <class T> class ncList; // forward declaration
template <class T> class ncListNode; // forward declaration

/// A List iterator
template <class T>
class ncListIterator
{
 public:
	ncListIterator(ncListNode<T> *pNode)
		: m_pNode(pNode) { }
	ncListIterator(const ncListIterator& iterator)
		: m_pNode(iterator.m_pNode) { }

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
	inline bool operator==(const ncListIterator<T>& iterator) const { return m_pNode == iterator.m_pNode; }
	/// Inequality operator
	inline bool operator!=(const ncListIterator<T>& iterator) const { return m_pNode != iterator.m_pNode; }

 private:
	mutable ncListNode<T> *m_pNode;

	friend class ncList<T>;
};

/// Read-only deferencing operator
template <class T>
const T& ncListIterator<T>::operator*() const
{
	// Cannot simply return only if m_pNode is not NULL or
	// "control may reach end of non-void function"
	return m_pNode->m_data;
}

/// Deferencing operator
template <class T>
T& ncListIterator<T>::operator*()
{
	// Cannot simply return only if m_pNode is not NULL or
	// "control may reach end of non-void function"
	return m_pNode->m_data;
}

/// Iterates to the next element (prefix)
template <class T>
ncListIterator<T> ncListIterator<T>::operator++() const
{
	if (m_pNode)
		m_pNode = m_pNode->m_pNext;

	return *this;
}

/// Iterates to the next element (postfix)
template <class T>
ncListIterator<T> ncListIterator<T>::operator++(int) const
{
	// Create an unmodified copy to return
	ncListIterator<T> iterator = *this;

	if (m_pNode)
		m_pNode = m_pNode->m_pNext;

	return iterator;
}

/// Iterates to the previous element (prefix)
template <class T>
ncListIterator<T> ncListIterator<T>::operator--() const
{
	if (m_pNode)
		m_pNode = m_pNode->m_pPrevious;

	return *this;
}

/// Iterates to the previous element (postfix)
template <class T>
ncListIterator<T> ncListIterator<T>::operator--(int) const
{
	// Create an unmodified copy to return
	ncListIterator<T> iterator = *this;

	if (m_pNode)
		m_pNode = m_pNode->m_pPrevious;

	return iterator;
}

#endif
