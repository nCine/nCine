#ifndef CLASS_NCLIST
#define CLASS_NCLIST

#include <cstdio> // for NULL

template <class T> class ncList; // forward declaration

/// A list node based on templates
template <class T>
class ncListNode
{
private:
	 ncListNode (const T& data, ncListNode *pNext)
		 : m_data(data), m_pNext(pNext) { }

	T m_data;
	ncListNode<T> *m_pNext;

	friend class ncList<T>;

public:
	/// Read-only deferencing operator
	inline const T& operator*() const { return m_data; }
	/// Deferencing operator
	inline T& operator*() { return m_data; }

	/// Constant reference to the node data
	inline const T& Data() const { return m_data; }
	/// Reference to the node data
	inline T& Data() { return m_data; }
	/// Pointer to the next node in the list
	inline ncListNode<T>* Next() const { return m_pNext; }
};


/// A list based on templates
template <class T>
class ncList
{
private:
	ncListNode<T> *m_pHead;
	// Preventing copy at the moment
	ncList(const ncList&);
	void operator=(const ncList&);

public:
	ncList() : m_pHead(NULL) { }
	~ncList() { Clear(); }

	/// Returns a pointer to the first node
	inline ncListNode<T>* Head() const { return m_pHead; }
	/// Returns true if the list is empty
	inline bool isEmpty() const { return m_pHead == NULL; }

	// Clears the list
	void Clear();
	// Inserts a new element as the first, in constant time
	void InsertFront(const T& element);
	// Inserts a new element as the last, in constant time
	void InsertBack(const T& element);
	// Inserts a new element after a specified one
	void InsertAfter(ncListNode<T> *pNode, const T& element);
	// Removes a specified element in constant time
	void Remove(const T& element);
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
}

/// Inserts a new element as the first, in constant time
template <class T>
void ncList<T>::InsertFront(const T& element)
{
	m_pHead = new ncListNode<T>(element, m_pHead);
}

/// Inserts a new element as the last, in constant time
template <class T>
void ncList<T>::InsertBack(const T& element)
{
	// The list is empty
	if (m_pHead == NULL)
		m_pHead = new ncListNode<T>(element, NULL);
	else
	{
		ncListNode<T> *pCurrent = m_pHead;
		ncListNode<T> *pNext = m_pHead->m_pNext;

		while(pNext)
		{
			pCurrent = pNext;
			pNext = pCurrent->m_pNext;
		}

		pCurrent->m_pNext = new ncListNode<T>(element, NULL);
	}
}

/// Inserts a new element after a specified one
template <class T>
void ncList<T>::InsertAfter(ncListNode<T> *pNode, const T& element)
{
	pNode->m_pNext = new ncListNode<T>(element, pNode->m_pNext);
}

/// Removes a specified element in constant time
template <class T>
void ncList<T>::Remove(const T& element)
{
	ncListNode<T> *pPrevious = NULL;
	ncListNode<T> *pCurrent = m_pHead;

	while(pCurrent)
	{
		if (pCurrent->m_data == element)
		{
			if (pPrevious)
				pPrevious->m_pNext = pCurrent->m_pNext;
			else // removing the head
				m_pHead = pCurrent->m_pNext;

			delete pCurrent;
			break;
		}

		pPrevious = pCurrent;
		pCurrent = pCurrent->m_pNext;
	}
}

#endif
