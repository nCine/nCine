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

	inline const T& Data() const { return m_data; }
	inline T& Data() { return m_data; }
	inline ncListNode<T>* Next() const { return m_pNext; }
};


/// A list based on templates
template <class T>
class ncList
{
private:
	ncListNode<T> *m_pHead;
public:
	ncList() : m_pHead(NULL) { }
	~ncList() { Clear(); }

	inline ncListNode<T>* Head() const { return m_pHead; }
	inline bool isEmpty() const { return m_pHead == NULL; }

	void Clear();
	void InsertFront(const T& element);
	void InsertBack(const T& element);
	void Remove(const T& element);
};

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

template <class T>
void ncList<T>::InsertFront(const T& element)
{
	m_pHead = new ncListNode<T>(element, m_pHead);
}

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
