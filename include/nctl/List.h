#ifndef CLASS_NCTL_LIST
#define CLASS_NCTL_LIST

#include "ListIterator.h"
#include "ReverseIterator.h"
#include "utility.h"

namespace nctl {

template <class T> class List;

/// The base list node used as sentinel
class BaseListNode
{
  public:
	/// A pointer to the previous node in the list
	BaseListNode *previous_;
	/// A pointer to the next node in the list
	BaseListNode *next_;

  protected:
	BaseListNode() : previous_(this), next_(this) { }

	BaseListNode(BaseListNode *previous, BaseListNode *next)
		: previous_(previous), next_(next) { }

	template<class T>
	friend class List;
};

/// A list node based on templates
template <class T>
class ListNode : public BaseListNode
{
  public:
	/// Data payload for the node
	T data_;

  private:
	ListNode(const T &data, BaseListNode *previous, BaseListNode *next)
		: BaseListNode(previous, next), data_(data) { }
	ListNode(T &&data, BaseListNode *previous, BaseListNode *next)
		: BaseListNode(previous, next), data_(nctl::move(data)) { }

	friend class List<T>;
};

/// A double linked list based on templates
template <class T>
class List
{
  public:
	/// Iterator type
	using Iterator = ListIterator<T, false>;
	/// Constant iterator type
	using ConstIterator = ListIterator<T, true>;
	/// Reverse iterator type
	using ReverseIterator = nctl::ReverseIterator<Iterator>;
	/// Reverse constant iterator type
	using ConstReverseIterator = nctl::ReverseIterator<ConstIterator>;

	List() : size_(0) { }
	~List() { clear(); }

	/// Copy constructor
	List(const List &other);
	/// Move constructor
	List(List &&other);
	/// Copy-and-swap assignment operator
	List &operator=(List other);

	/// Swaps two lists without copying their data
	void swap(List &first, List &second)
	{
		nctl::swap(first.size_, second.size_);
		nctl::swap(first.sentinel_.previous_, second.sentinel_.previous_);
		nctl::swap(first.sentinel_.next_, second.sentinel_.next_);
	}

	/// Returns an iterator to the first element
	inline Iterator begin() { return Iterator(static_cast<ListNode<T> *>(sentinel_.next_)); }
	/// Returns a reverse iterator to the last element
	inline ReverseIterator rBegin() { return ReverseIterator(Iterator(static_cast<ListNode<T> *>(sentinel_.previous_))); }
	/// Returns an iterator to the end of the list sentinel (valid for reverse traversal too)
	inline Iterator end() { return Iterator(static_cast<ListNode<T> *>(&sentinel_)); }
	/// Returns a reverse iterator to the end of the list sentinel
	inline ReverseIterator rEnd() { return ReverseIterator(end()); }

	/// Returns a constant iterator to the first element
	inline ConstIterator begin() const { return ConstIterator(static_cast<ListNode<T> *>(sentinel_.next_)); }
	/// Returns a constant reverse iterator to the last element
	inline ConstReverseIterator rBegin() const { return ConstReverseIterator(ConstIterator(static_cast<ListNode<T> *>(sentinel_.previous_))); }
	/// Returns a constant iterator to the end of the list sentinel (valid reverse traversal too)
	inline ConstIterator end() const { return ConstIterator(static_cast<ListNode<T> *>(const_cast<BaseListNode *>(&sentinel_))); }
	/// Returns a constant reverse iterator to the end of the list sentinel
	inline ConstReverseIterator rEnd() const { return ConstReverseIterator(end()); }

	/// Returns a constant iterator to the first element
	inline ConstIterator cBegin() const { return ConstIterator(static_cast<ListNode<T> *>(sentinel_.next_)); }
	/// Returns a constant reverse iterator to the last element
	inline ConstReverseIterator crBegin() const { return ConstReverseIterator(ConstIterator(static_cast<ListNode<T> *>(sentinel_.previous_))); }
	/// Returns a constant iterator to the end of the list sentinel (valid reverse traversal too)
	inline ConstIterator cEnd() const { return ConstIterator(static_cast<ListNode<T> *>(const_cast<BaseListNode *>(&sentinel_))); }
	/// Returns a constant reverse iterator to the end of the list sentinel
	inline ConstReverseIterator crEnd() const { return ConstReverseIterator(end()); }

	/// Returns true if the list is empty
	inline bool isEmpty() const { return (sentinel_.next_ == &sentinel_); }
	/// Returns the number of elements in the list
	inline unsigned int size() const { return size_; }
	/// Clears the list
	void clear();
	/// Returns a constant reference to the first element in constant time
	inline const T &front() const { ASSERT(sentinel_.next_ != &sentinel_); return static_cast<ListNode<T> *>(sentinel_.next_)->data_; }
	/// Returns a reference to the first element in constant time
	inline T &front() { ASSERT(sentinel_.next_ != &sentinel_); return static_cast<ListNode<T> *>(sentinel_.next_)->data_; }
	/// Returns a constant reference to the last element in constant time
	inline const T &back() const { ASSERT(sentinel_.previous_ != &sentinel_); return static_cast<ListNode<T> *>(sentinel_.previous_)->data_; }
	/// Returns a reference to the last element in constant time
	inline T &back() { ASSERT(sentinel_.previous_ != &sentinel_); return static_cast<ListNode<T> *>(sentinel_.previous_)->data_; }
	/// Inserts a new element as the first, in constant time
	inline void pushFront(const T &element) { insertBeforeNode(static_cast<ListNode<T> *>(sentinel_.next_), element); }
	/// Move inserts a new element as the first, in constant time
	inline void pushFront(T &&element) { insertBeforeNode(static_cast<ListNode<T> *>(sentinel_.next_), nctl::move(element)); }
	/// Inserts a new element as the last, in constant time
	inline void pushBack(const T &element) { insertAfterNode(static_cast<ListNode<T> *>(sentinel_.previous_), element); }
	/// Move inserts a new element as the last, in constant time
	inline void pushBack(T &&element) { insertAfterNode(static_cast<ListNode<T> *>(sentinel_.previous_), nctl::move(element)); }
	/// Removes the first element in constant time
	inline void popFront() { removeNode(sentinel_.next_); }
	/// Removes the last element in constant time
	inline void popBack() { removeNode(sentinel_.previous_); }
	/// Inserts a new element after the node pointed by the constant iterator
	ConstIterator insertAfter(const Iterator position, const T &element);
	/// Move inserts a new element after the node pointed by the constant iterator
	ConstIterator insertAfter(const Iterator position, T &&element);
	/// Inserts a new element before the node pointed by the constant iterator
	ConstIterator insertBefore(const Iterator position, const T &element);
	/// Move inserts a new element before the node pointed by the constant iterator
	ConstIterator insertBefore(const Iterator position, T &&element);
	/// Inserts new elements from a source range after the node pointed by the constant iterator, last not included
	ConstIterator insert(const Iterator position, Iterator first, const Iterator last);
	/// Removes the node pointed by the constant iterator in constant time
	ConstIterator erase(ConstIterator position);
	/// Removes the range of nodes pointed by the iterators in constant time
	ConstIterator erase(ConstIterator first, const ConstIterator last);
	/// Removes a specified element in linear time
	void remove(const T &element);
	/// Removes all the elements that fulfill the condition
	template <class Predicate> void removeIf(Predicate pred);
	/// Transfers all the elements from the source list in front of `position`
	void splice(Iterator position, List &source);
	/// Transfers one element at `it` from the source list in front of `position`
	void splice(Iterator position, List &source, Iterator it);
	/// Transfers a range of elements from the source list, `last` not included, in front of `position`
	void splice(Iterator position, List &source, Iterator first, Iterator last);

  private:
	/// Number of elements in the list
	unsigned int size_;
	/// The sentinel node
	BaseListNode sentinel_;

	/// Inserts a new element after a specified node
	ListNode<T> *insertAfterNode(ListNode<T> *node, const T &element);
	/// Move inserts a new element after a specified node
	ListNode<T> *insertAfterNode(ListNode<T> *node, T &&element);
	/// Inserts a new element before a specified node
	ListNode<T> *insertBeforeNode(ListNode<T> *node, const T &element);
	/// Move inserts a new element before a specified node
	ListNode<T> *insertBeforeNode(ListNode<T> *node, T &&element);
	/// Removes a specified node in constant time
	ListNode<T> *removeNode(BaseListNode *node);
	/// Removes a range of nodes in constant time, last not included
	ListNode<T> *removeRange(ListNode<T> *firstNode, ListNode<T> *lastNode);
};

template <class T>
List<T>::List(const List<T> &other)
	: size_(0)
{
	for (List<T>::ConstIterator i = other.begin(); i != other.end(); ++i)
		pushBack(*i);
}

template <class T>
List<T>::List(List<T> &&other)
	: size_(other.size_)
{
	if (other.size_ > 0)
	{
		sentinel_.previous_ = other.sentinel_.previous_;
		sentinel_.next_ = other.sentinel_.next_;
		sentinel_.previous_->next_ = &sentinel_;
		sentinel_.next_->previous_ = &sentinel_;

		other.size_ = 0;
		other.sentinel_.previous_ = &other.sentinel_;
		other.sentinel_.next_ = &other.sentinel_;
	}
}

/*! \note The parameter should be passed by value for the idiom to work. */
template <class T>
List<T> &List<T>::operator=(List<T> other)
{
	if (size_ == 0)
	{
		sentinel_.previous_ = &other.sentinel_;
		sentinel_.next_ = &other.sentinel_;
	}
	if (other.size_ == 0)
	{
		other.sentinel_.previous_ = &sentinel_;
		other.sentinel_.next_ = &sentinel_;
	}

	swap(*this, other);

	sentinel_.previous_->next_ = &sentinel_;
	sentinel_.next_->previous_ = &sentinel_;
	other.sentinel_.previous_->next_ = &other.sentinel_;
	other.sentinel_.next_->previous_ = &other.sentinel_;

	return *this;
}

template <class T>
void List<T>::clear()
{
	BaseListNode *nextNode = sentinel_.next_;

	while (nextNode != &sentinel_)
	{
		nextNode = nextNode->next_;
		// Cast is needed to prevent memory leaking
		delete static_cast<ListNode<T> *>(sentinel_.next_);
		sentinel_.next_ = nextNode;
	}

	sentinel_.previous_ = &sentinel_;
	size_ = 0;
}

template <class T>
inline typename List<T>::ConstIterator List<T>::insertAfter(const Iterator position, const T &element)
{
	return ConstIterator(insertAfterNode(position.node_, element));
}

template <class T>
inline typename List<T>::ConstIterator List<T>::insertAfter(const Iterator position, T &&element)
{
	return ConstIterator(insertAfterNode(position.node_, nctl::move(element)));
}

template <class T>
inline typename List<T>::ConstIterator List<T>::insertBefore(const Iterator position, const T &element)
{
	return ConstIterator(insertBeforeNode(position.node_, element));
}

template <class T>
inline typename List<T>::ConstIterator List<T>::insertBefore(const Iterator position, T &&element)
{
	return ConstIterator(insertBeforeNode(position.node_, nctl::move(element)));
}

template <class T>
typename List<T>::ConstIterator List<T>::insert(Iterator position, Iterator first, const Iterator last)
{
	ListNode<T> *node = position.node_;
	while (first != last)
	{
		node = insertAfterNode(node, *first);
		++first;
	}

	return ConstIterator(node);
}

/*! \note The iterator cannot be used after on. */
template <class T>
inline typename List<T>::ConstIterator List<T>::erase(ConstIterator position)
{
	ListNode<T> *nextNode = removeNode(position.node_);
	return ConstIterator(nextNode);
}

/*! \note The first iterator cannot be used after on. */
template <class T>
inline typename List<T>::ConstIterator List<T>::erase(ConstIterator first, const ConstIterator last)
{
	ListNode<T> *nextNode = removeRange(first.node_, last.node_);
	return ConstIterator(nextNode);
}

template <class T>
void List<T>::remove(const T &element)
{
	BaseListNode *current = sentinel_.next_;

	while (current != &sentinel_)
	{
		ListNode<T> *dataNode = static_cast<ListNode<T> *>(current);
		if (dataNode->data_ == element)
		{
			removeNode(dataNode);
			break;
		}

		current = current->next_;
	}
}

template <class T> template <class Predicate>
void List<T>::removeIf(Predicate pred)
{
	ConstIterator i = begin();
	while (i != end())
	{
		if (pred(*i))
			i = erase(i);
		else
			++i;
	}
}

template <class T>
void List<T>::splice(Iterator position, List &source)
{
	splice(position, source, source.begin(), source.end());
}

template <class T>
void List<T>::splice(Iterator position, List &source, Iterator it)
{
	Iterator next = it;
	splice(position, source, it, ++next);
}

template <class T>
void List<T>::splice(Iterator position, List &source, Iterator first, Iterator last)
{
	// Early-out if the source list is empty
	if (source.isEmpty())
		return;

	BaseListNode *node = position.node_;
	BaseListNode *firstNode = first.node_;

	BaseListNode *firstPrev = firstNode->previous_;
	BaseListNode *lastIncludedNode = first.node_;
	while (first != last)
	{
		lastIncludedNode = first.node_;

		++first;
		size_++;
		source.size_--;
	}

	// it also works if `firstPrev` is the source sentinel
	firstPrev->next_ = lastIncludedNode->next_;
	// it also works if `lastIncludedNode` is the source sentinel
	lastIncludedNode->next_->previous_ = firstPrev;

	BaseListNode *prevNode = node->previous_;
	node->previous_ = lastIncludedNode;
	firstNode->previous_ = prevNode;
	lastIncludedNode->next_ = node;
	// it also works if `prevNode` is the sentinel
	prevNode->next_ = firstNode;

	lastIncludedNode->next_ = node;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

template <class T>
ListNode<T> *List<T>::insertAfterNode(ListNode<T> *node, const T &element)
{
	ListNode<T> *newNode = new ListNode<T>(element, node, node->next_);

	// it also works if `node->next_` is the sentinel
	node->next_->previous_ = newNode;
	node->next_ = newNode;
	size_++;

	return newNode;
}

template <class T>
ListNode<T> *List<T>::insertAfterNode(ListNode<T> *node, T &&element)
{
	ListNode<T> *newNode = new ListNode<T>(nctl::move(element), node, node->next_);

	// it also works if `node->next_` is the sentinel
	node->next_->previous_ = newNode;
	node->next_ = newNode;
	size_++;

	return newNode;
}

template <class T>
ListNode<T> *List<T>::insertBeforeNode(ListNode<T> *node, const T &element)
{
	ListNode<T> *newNode = new ListNode<T>(element, node->previous_, node);

	// it also works if `node->previous_` is the sentinel
	node->previous_->next_ = newNode;
	node->previous_ = newNode;
	size_++;

	return newNode;
}

template <class T>
ListNode<T> *List<T>::insertBeforeNode(ListNode<T> *node, T &&element)
{
	ListNode<T> *newNode = new ListNode<T>(nctl::move(element), node->previous_, node);

	// it also works if `node->previous_` is the sentinel
	node->previous_->next_ = newNode;
	node->previous_ = newNode;
	size_++;

	return newNode;
}

template <class T>
ListNode<T> *List<T>::removeNode(BaseListNode *node)
{
	ListNode<T> *firstNode = static_cast<ListNode<T> *>(node);
	ListNode<T> *lastNode = static_cast<ListNode<T> *>(node->next_);
	return removeRange(firstNode, lastNode);
}

template <class T>
ListNode<T> *List<T>::removeRange(ListNode<T> *firstNode, ListNode<T> *lastNode)
{
	BaseListNode *previous = firstNode->previous_;
	BaseListNode *next = &sentinel_;
	BaseListNode *current = firstNode;
	while (current != lastNode)
	{
		next = current->next_;
		// Cast is needed to prevent memory leaking
		delete static_cast<ListNode<T> *>(current);
		size_--;
		current = next;
	}

	// if `previous` is the sentinel, the head is going to be removed
	previous->next_ = lastNode;
	// if `lastNode` is the sentinel, the tail is going to be removed
	lastNode->previous_ = previous;

	return lastNode;
}

}

#endif
