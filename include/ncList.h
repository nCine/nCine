#ifndef CLASS_NCLIST
#define CLASS_NCLIST

#include <cstdio> // for NULL
#include <ncListIterator.h>

template <class T> class ncList; // forward declaration

/// A list node based on templates
template <class T>
class ncListNode
{
private:
	 ncListNode(const T& data, ncListNode *pPrevious, ncListNode *pNext)
		 : m_data(data), m_pPrevious(pPrevious), m_pNext(pNext) { }

	 friend class ncList<T>;
public:
	/// Data payload for the node
	T m_data;
	/// A pointer to the previous node in the list
	ncListNode *m_pPrevious;
	/// A pointer to the next node in the list
	ncListNode *m_pNext;
};


/// A double linked list based on templates
template <class T>
class ncList
{
private:
	/// Pointer to the first node in the list
	ncListNode<T> *m_pHead;
	/// Pointer to the last node in the list
	ncListNode<T> *m_pTail;

	/// Private copy constructor (preventing copy at the moment)
	ncList(const ncList&);
	/// Private assignment operator (preventing copy at the moment)
	ncList& operator=(const ncList&);

	// Inserts a new element after a specified node
	void InsertAfter(ncListNode<T> *pNode, const T& element);
	// Inserts a new element before a specified node
	void InsertBefore(ncListNode<T> *pNode, const T& element);
	// Removes a specified node in constant time
	void Remove(ncListNode<T> *pNode);

	friend class ncListIterator<T>;
public:
	ncList() : m_pHead(NULL), m_pTail(NULL) { }
	~ncList() { Clear(); }

	/// Iterator type
	typedef ncListIterator<T> Iterator;
	/// Constant iterator type
	typedef const ncListIterator<T> Const_Iterator;

	/// Returns true if the list is empty
	inline bool isEmpty() const { return m_pHead == NULL; }

	/// Returns an iterator to the first element
	inline Iterator Begin() { return Iterator(m_pHead); }
	/// Returns an iterator to the last element
	inline Iterator RBegin() { return Iterator(m_pTail); }
	/// Returns an iterator to the end of the list sentinel (reverse traversal too)
	inline Iterator End() { return Iterator(NULL); }

	/// Returns a constant iterator to the first element
	inline Const_Iterator Begin() const { return Iterator(m_pHead); }
	/// Returns a constant iterator to the last element
	inline Const_Iterator RBegin() const { return Iterator(m_pTail); }
	/// Returns a constant iterator to the end of the list sentinel (reverse traversal too)
	inline Const_Iterator End() const { return Iterator(NULL); }

	// Clears the list
	void Clear();
	// Inserts a new element as the first, in constant time
	void InsertFront(const T& element);
	// Inserts a new element as the last, in constant time
	void InsertBack(const T& element);
	// Inserts a new element after the node pointed by the iterator
	inline void InsertAfter(Iterator it, const T& element);
	// Inserts a new element before the node pointed by the iterator
	inline void InsertBefore(Iterator it, const T& element);
	// Removes the node pointed by the iterator in constant time
	void Remove(Iterator it);
	// Removes a specified element in linear time
	void Remove(const T& element);
	// Removes the first element in constant time
	T RemoveFront();
	// Removes the last element in constant time
	T RemoveBack();
};

/// Clears the list
template <class T>
void ncList<T>::Clear()
{
	ncListNode<T> *pNext = NULL;

	while(m_pHead)
	{
		pNext = m_pHead->m_pNext;
		delete m_pHead;
		m_pHead = pNext;
	}

	m_pTail = NULL;
}

/// Inserts a new element as the first, in constant time
template <class T>
void ncList<T>::InsertFront(const T& element)
{
	ncListNode<T> *pNode = new ncListNode<T>(element, NULL, m_pHead);
	if (m_pHead)
		m_pHead->m_pPrevious = pNode;
	m_pHead = pNode;

	// The list is empty
	if (m_pTail == NULL)
		m_pTail = pNode;
}

/// Inserts a new element as the last, in constant time
template <class T>
void ncList<T>::InsertBack(const T& element)
{
	ncListNode<T> *pNode = new ncListNode<T>(element, m_pTail, NULL);
	if (m_pTail)
		m_pTail->m_pNext = pNode;
	m_pTail = pNode;

	// The list is empty
	if (m_pHead == NULL)
		m_pHead = pNode;
}

/// Inserts a new element after the node pointed by the iterator
template <class T>
void ncList<T>::InsertAfter(Iterator it, const T& element)
{
	InsertAfter(it.m_pNode);
}

/// Inserts a new element before the node pointed by the iterator
template <class T>
void ncList<T>::InsertBefore(Iterator it, const T& element)
{
	InsertBefore(it.m_pNode);
}

/// Removes the node pointed by the iterator in constant time
/*! Note: The iterator cannot be used after on */
template <class T>
void ncList<T>::Remove(Iterator it)
{
	Remove(it.m_pNode);
	it.m_pNode = NULL;
}

/// Removes a specified element in linear time
template <class T>
void ncList<T>::Remove(const T& element)
{
	ncListNode<T> *pCurrent = m_pHead;

	while(pCurrent)
	{
		if (pCurrent->m_data == element)
		{
			Remove(pCurrent);
			break;
		}

		pCurrent = pCurrent->m_pNext;
	}
}

/// Removes the first element in constant time
template <class T>
T ncList<T>::RemoveFront()
{
	T first;

	if (m_pHead)
	{
		// Saving the old head pointer to delete it
		ncListNode<T> *pOldHead = m_pHead;

		first = m_pHead->m_data;
		if (m_pHead->m_pNext)
			m_pHead->m_pNext->m_pPrevious = NULL;
		else
			m_pTail = NULL;
		m_pHead = m_pHead->m_pNext;

		delete pOldHead;
	}

	return first;
}

/// Removes the last element in constant time
template <class T>
T ncList<T>::RemoveBack()
{
	T last;

	if (m_pTail)
	{
		// Saving the old tail pointer to delete it
		ncListNode<T> *pOldTail = m_pTail;

		last = m_pTail->m_data;
		if (m_pTail->m_pPrevious)
			m_pTail->m_pPrevious->m_pNext = NULL;
		else
			m_pHead = NULL;
		m_pTail = m_pTail->m_pPrevious;

		delete pOldTail;
	}

	return last;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Inserts a new element after a specified node
template <class T>
void ncList<T>::InsertAfter(ncListNode<T> *pNode, const T& element)
{
	ncListNode<T> *pNewNode = new ncListNode<T>(element, pNode, pNode->m_pNext);

	if (pNode->m_pNext == NULL)
		m_pTail = pNewNode;
	else
		pNode->m_pNext->m_pPrevious = pNewNode;

	pNode->m_pNext = pNewNode;
}

/// Inserts a new element before a specified node
template <class T>
void ncList<T>::InsertBefore(ncListNode<T> *pNode, const T& element)
{
	ncListNode<T> *pNewNode = new ncListNode<T>(element, pNode->m_pPrevious, pNode);

	if (pNode->m_pPrevious == NULL)
		m_pHead = pNewNode;
	else
		pNode->m_pPrevious->m_pNext = pNewNode;

	pNode->m_pPrevious = pNewNode;
}

/// Removes a specified node in constant time
template <class T>
void ncList<T>::Remove(ncListNode<T> *pNode)
{
	// Preventing NULL deletion
	if (pNode == NULL)
		return;

	if (pNode->m_pPrevious)
		pNode->m_pPrevious->m_pNext = pNode->m_pNext;
	else // removing the head
		m_pHead = pNode->m_pNext;

	if (pNode->m_pNext)
		pNode->m_pNext->m_pPrevious = pNode->m_pPrevious;
	else // removing the tail
		m_pTail = pNode->m_pPrevious;

	delete pNode;
}


#endif
