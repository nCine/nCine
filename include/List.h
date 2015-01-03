#ifndef CLASS_NCINE_LIST
#define CLASS_NCINE_LIST

#include <cstdio> // for NULL
#include "common.h"
#include <ListIterator.h>

namespace ncine {

template <class T> class List;

/// A list node based on templates
template <class T>
class ListNode
{
  public:
	/// Data payload for the node
	T data_;
	/// A pointer to the previous node in the list
	ListNode *previous_;
	/// A pointer to the next node in the list
	ListNode *next_;

  private:
	ListNode(const T& data, ListNode *previous, ListNode *next)
		: data_(data), previous_(previous), next_(next) { }

	friend class List<T>;
};


/// A double linked list based on templates
template <class T>
class List
{
  public:
	/// Iterator type
	typedef ListIterator<T> Iterator;
	/// Constant iterator type
	typedef const ListIterator<T> Const_Iterator;

	List() : head_(NULL), tail_(NULL) { }
	~List() { clear(); }

	// Copy constructor
	List(const List& other);
	// Copy-and-swap assignment operator
	List& operator=(List other);

	/// Swaps two lists without copying their data
	void swap(List& first, List& second)
	{
		nc::swap(first.head_, second.head_);
		nc::swap(first.tail_, second.tail_);
	}

	/// Returns an iterator to the first element
	inline Iterator begin() { return Iterator(head_); }
	/// Returns an iterator to the last element
	inline Iterator rBegin() { return Iterator(tail_); }
	/// Returns an iterator to the end of the list sentinel (valid for reverse traversal too)
	inline Iterator end() { return Iterator(NULL); }
	/// Returns an iterator to the end of the list sentinel
	/** It exists only for coherency with the rest of the containers */
	inline Iterator rEnd() { return end(); }

	/// Returns a constant iterator to the first element
	inline Const_Iterator begin() const { return Iterator(head_); }
	/// Returns a constant iterator to the last element
	inline Const_Iterator rBegin() const { return Iterator(tail_); }
	/// Returns a constant iterator to the end of the list sentinel (valid reverse traversal too)
	inline Const_Iterator end() const { return Iterator(NULL); }
	/// Returns a constant iterator to the end of the list sentinel
	/** It exists only for coherency with the rest of the containers */
	inline Const_Iterator rEnd() const { return end(); }

	/// Returns true if the list is empty
	inline bool isEmpty() const { return head_ == NULL; }
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
	ListNode<T> *head_;
	/// Pointer to the last node in the list
	ListNode<T> *tail_;

	// Inserts a new element after a specified node
	void insertAfter(ListNode<T> *node, const T& element);
	// Inserts a new element before a specified node
	void insertBefore(ListNode<T> *node, const T& element);
	// Removes a specified node in constant time
	void remove(ListNode<T> *node);

	friend class ListIterator<T>;
};

/// Copy constructor
template <class T>
List<T>::List(const List<T>& other)
	: head_(NULL), tail_(NULL)
{
	for (List<T>::Const_Iterator i = other.begin(); i != other.end(); ++i)
	{
		T element(*i);
		insertBack(element);
	}
}

/// Copy-and-swap assignment operator
/** The parameter should be passed by value for the idiom to work */
template <class T>
List<T>& List<T>::operator=(List<T> other)
{
	swap(*this, other);
	return *this;
}

/// Clears the list
template <class T>
void List<T>::clear()
{
	ListNode<T> *next = NULL;

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
void List<T>::insertFront(const T& element)
{
	ListNode<T> *node = new ListNode<T>(element, NULL, head_);
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
void List<T>::insertBack(const T &element)
{
	ListNode<T> *node = new ListNode<T>(element, tail_, NULL);
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
void List<T>::insertAfter(Iterator it, const T& element)
{
	insertAfter(it.node_);
}

/// Inserts a new element before the node pointed by the iterator
template <class T>
void List<T>::insertBefore(Iterator it, const T& element)
{
	insertBefore(it.node_);
}

/// Removes the node pointed by the iterator in constant time
/*! Note: The iterator cannot be used after on */
template <class T>
void List<T>::remove(Iterator it)
{
	remove(it.node_);
	it.node_ = NULL;
}

/// Removes a specified element in linear time
template <class T>
void List<T>::remove(const T& element)
{
	ListNode<T> *current = head_;

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
T List<T>::removeFront()
{
	T first = T();

	if (head_)
	{
		// Saving the old head pointer to delete it
		ListNode<T> *oldHead = head_;

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
T List<T>::removeBack()
{
	T last = T();

	if (tail_)
	{
		// Saving the old tail pointer to delete it
		ListNode<T> *oldTail = tail_;

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
void List<T>::insertAfter(ListNode<T> *node, const T& element)
{
	ListNode<T> *newNode = new ListNode<T>(element, node, node->next_);

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
void List<T>::insertBefore(ListNode<T> *node, const T& element)
{
	ListNode<T> *newNode = new ListNode<T>(element, node->previous_, node);

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
void List<T>::remove(ListNode<T> *node)
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

}

#endif
