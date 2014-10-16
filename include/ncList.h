#ifndef CLASS_NCLIST
#define CLASS_NCLIST

#include <cstdio> // for NULL
#include <ncListIterator.h>

template <class T> class ncList; // forward declaration

/// A list node based on templates
template <class T>
class ncListNode
{
  public:
	/// Data payload for the node
	T data_;
	/// A pointer to the previous node in the list
	ncListNode *previous_;
	/// A pointer to the next node in the list
	ncListNode *next_;

  private:
	ncListNode(const T& data, ncListNode *previous, ncListNode *next)
		: data_(data), previous_(previous), next_(next) { }

	friend class ncList<T>;
};


/// A double linked list based on templates
template <class T>
class ncList
{
  public:
	/// Iterator type
	typedef ncListIterator<T> Iterator;
	/// Constant iterator type
	typedef const ncListIterator<T> Const_Iterator;

	ncList() : head_(NULL), tail_(NULL) { }
	~ncList() { clear(); }

	/// Returns true if the list is empty
	inline bool isEmpty() const { return head_ == NULL; }

	/// Returns an iterator to the first element
	inline Iterator begin() { return Iterator(head_); }
	/// Returns an iterator to the last element
	inline Iterator revBegin() { return Iterator(tail_); }
	/// Returns an iterator to the end of the list sentinel (reverse traversal too)
	inline Iterator end() { return Iterator(NULL); }

	/// Returns a constant iterator to the first element
	inline Const_Iterator begin() const { return Iterator(head_); }
	/// Returns a constant iterator to the last element
	inline Const_Iterator revBegin() const { return Iterator(tail_); }
	/// Returns a constant iterator to the end of the list sentinel (reverse traversal too)
	inline Const_Iterator end() const { return Iterator(NULL); }

	// Clears the list
	void clear();
	// Inserts a new element as the first, in constant time
	void insertFront(const T& element);
	// Inserts a new element as the last, in constant time
	void insertBack(const T& element);
	// Inserts a new element after the node pointed by the iterator
	inline void insertAfter(Iterator it, const T& element);
	// Inserts a new element before the node pointed by the iterator
	inline void insertBefore(Iterator it, const T& element);
	// Removes the node pointed by the iterator in constant time
	void remove(Iterator it);
	// Removes a specified element in linear time
	void remove(const T& element);
	// Removes the first element in constant time
	T removeFront();
	// Removes the last element in constant time
	T removeBack();

  private:
	/// Pointer to the first node in the list
	ncListNode<T> *head_;
	/// Pointer to the last node in the list
	ncListNode<T> *tail_;

	/// Private copy constructor (preventing copy at the moment)
	ncList(const ncList&);
	/// Private assignment operator (preventing copy at the moment)
	ncList& operator=(const ncList&);

	// Inserts a new element after a specified node
	void insertAfter(ncListNode<T> *node, const T& element);
	// Inserts a new element before a specified node
	void insertBefore(ncListNode<T> *node, const T& element);
	// Removes a specified node in constant time
	void remove(ncListNode<T> *node);

	friend class ncListIterator<T>;
};

/// Clears the list
template <class T>
void ncList<T>::clear()
{
	ncListNode<T> *next = NULL;

	while (head_)
	{
		next = head_->next_;
		delete head_;
		head_ = next;
	}

	tail_ = NULL;
}

/// Inserts a new element as the first, in constant time
template <class T>
void ncList<T>::insertFront(const T& element)
{
	ncListNode<T> *node = new ncListNode<T>(element, NULL, head_);
	if (head_)
	{
		head_->previous_ = node;
	}
	head_ = node;

	// The list is empty
	if (tail_ == NULL)
	{
		tail_ = node;
	}
}

/// Inserts a new element as the last, in constant time
template <class T>
void ncList<T>::insertBack(const T &element)
{
	ncListNode<T> *node = new ncListNode<T>(element, tail_, NULL);
	if (tail_)
	{
		tail_->next_ = node;
	}
	tail_ = node;

	// The list is empty
	if (head_ == NULL)
	{
		head_ = node;
	}
}

/// Inserts a new element after the node pointed by the iterator
template <class T>
void ncList<T>::insertAfter(Iterator it, const T& element)
{
	insertAfter(it.node_);
}

/// Inserts a new element before the node pointed by the iterator
template <class T>
void ncList<T>::insertBefore(Iterator it, const T& element)
{
	insertBefore(it.node_);
}

/// Removes the node pointed by the iterator in constant time
/*! Note: The iterator cannot be used after on */
template <class T>
void ncList<T>::remove(Iterator it)
{
	remove(it.node_);
	it.node_ = NULL;
}

/// Removes a specified element in linear time
template <class T>
void ncList<T>::remove(const T& element)
{
	ncListNode<T> *current = head_;

	while (current)
	{
		if (current->data_ == element)
		{
			remove(current);
			break;
		}

		current = current->next_;
	}
}

/// Removes the first element in constant time
template <class T>
T ncList<T>::removeFront()
{
	T first;

	if (head_)
	{
		// Saving the old head pointer to delete it
		ncListNode<T> *oldHead = head_;

		first = head_->data_;
		if (head_->next_)
		{
			head_->next_->previous_ = NULL;
		}
		else
		{
			tail_ = NULL;
		}
		head_ = head_->next_;

		delete oldHead;
	}

	return first;
}

/// Removes the last element in constant time
template <class T>
T ncList<T>::removeBack()
{
	T last;

	if (tail_)
	{
		// Saving the old tail pointer to delete it
		ncListNode<T> *oldTail = tail_;

		last = tail_->data_;
		if (tail_->previous_)
		{
			tail_->previous_->next_ = NULL;
		}
		else
		{
			head_ = NULL;
		}
		tail_ = tail_->previous_;

		delete oldTail;
	}

	return last;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Inserts a new element after a specified node
template <class T>
void ncList<T>::insertAfter(ncListNode<T> *node, const T& element)
{
	ncListNode<T> *newNode = new ncListNode<T>(element, node, node->next_);

	if (node->next_ == NULL)
	{
		tail_ = newNode;
	}
	else
	{
		node->next_->previous_ = newNode;
	}

	node->next_ = newNode;
}

/// Inserts a new element before a specified node
template <class T>
void ncList<T>::insertBefore(ncListNode<T> *node, const T& element)
{
	ncListNode<T> *newNode = new ncListNode<T>(element, node->previous_, node);

	if (node->previous_ == NULL)
	{
		head_ = newNode;
	}
	else
	{
		node->previous_->next_ = newNode;
	}

	node->previous_ = newNode;
}

/// Removes a specified node in constant time
template <class T>
void ncList<T>::remove(ncListNode<T> *node)
{
	// Preventing NULL deletion
	if (node == NULL)
	{
		return;
	}

	if (node->previous_)
	{
		node->previous_->next_ = node->next_;
	}
	else // removing the head
	{
		head_ = node->next_;
	}

	if (node->next_)
	{
		node->next_->previous_ = node->previous_;
	}
	else // removing the tail
	{
		tail_ = node->previous_;
	}

	delete node;
}

#endif
