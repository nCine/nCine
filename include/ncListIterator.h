#ifndef CLASS_NCLISTITERATOR
#define CLASS_NCLISTITERATOR

template <class T> class ncList; // forward declaration
template <class T> class ncListNode; // forward declaration

/// A List iterator
template <class T>
class ncListIterator
{
private:
	mutable ncListNode<T> *m_pNode;

public:
	ncListIterator(ncListNode<T> *pNode)
		: m_pNode(pNode) { }
	ncListIterator(const ncListIterator& iterator)
		: m_pNode(iterator.m_pNode) { }

	// Read-only deferencing operator
	inline const T& operator*() const;
	// Deferencing operator
	inline T& operator*();

	// Iterate to the next element (prefix)
	ncListIterator<T> operator++() const;
	// Iterate to the next element (postfix)
	ncListIterator<T> operator++(int) const;

	// Iterate to the previous element (prefix)
	ncListIterator<T> operator--() const;
	// Iterate to the previous element (postfix)
	ncListIterator<T> operator--(int) const;

	/// Equality operator
	inline bool operator==(const ncListIterator<T>& iterator) const { return m_pNode == iterator.m_pNode; }
	/// Inequality operator
	inline bool operator!=(const ncListIterator<T>& iterator) const { return m_pNode != iterator.m_pNode; }

	friend class ncList<T>;
};

/// Read-only deferencing operator
template <class T>
const T& ncListIterator<T>::operator*() const
{
	if (m_pNode)
		return m_pNode->m_data;
}
/// Deferencing operator
template <class T>
T& ncListIterator<T>::operator*()
{
	if (m_pNode)
		return m_pNode->m_data;
}

/// Iterate to the next element (prefix)
template <class T>
ncListIterator<T> ncListIterator<T>::operator++() const
{
	if (m_pNode)
		m_pNode = m_pNode->m_pNext;

	return *this;
}

/// Iterate to the next element (postfix)
template <class T>
ncListIterator<T> ncListIterator<T>::operator++(int) const
{
	// Create an unmodified copy to return
	ncListIterator<T> iterator = *this;

	if (m_pNode)
		m_pNode = m_pNode->m_pNext;

	return iterator;
}

/// Iterate to the previous element (prefix)
template <class T>
ncListIterator<T> ncListIterator<T>::operator--() const
{
	if (m_pNode)
		m_pNode = m_pNode->m_pPrevious;

	return *this;
}

/// Iterate to the previous element (postfix)
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
