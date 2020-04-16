#ifndef CLASS_NCTL_LIST
#define CLASS_NCTL_LIST

#include "ListIterator.h"
#include "ReverseIterator.h"
#include "utility.h"

#include <ncine/config.h>
#if NCINE_WITH_ALLOCATORS
	#include "AllocManager.h"
	#include "IAllocator.h"
#endif

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
	BaseListNode()
	    : previous_(this), next_(this) {}

	BaseListNode(BaseListNode *previous, BaseListNode *next)
	    : previous_(previous), next_(next) {}

	template <class T>
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
	ListNode(BaseListNode *previous, BaseListNode *next, const T &data)
	    : BaseListNode(previous, next), data_(data) {}
	ListNode(BaseListNode *previous, BaseListNode *next, T &&data)
	    : BaseListNode(previous, next), data_(nctl::move(data)) {}
	template <typename... Args>
	ListNode(BaseListNode *previous, BaseListNode *next, Args &&... args)
	    : BaseListNode(previous, next), data_(nctl::forward<Args>(args)...) {}

	friend class List<T>;
#if NCINE_WITH_ALLOCATORS
	friend struct detail::allocHelpers<false>;
#endif
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

#if !NCINE_WITH_ALLOCATORS
	List()
	    : size_(0) {}
#else
	List()
	    : List(theDefaultAllocator()) {}

	explicit List(IAllocator &alloc)
	    : alloc_(alloc), size_(0) {}
#endif
	~List() { clear(); }

	/// Copy constructor
	List(const List &other);
	/// Move constructor
	List(List &&other);
	/// Assignment operator
	List &operator=(const List &other);
	/// Move assignment operator
	List &operator=(List &&other);

	/// Swaps two lists without copying their data
	inline void swap(List &first, List &second)
	{
#if NCINE_WITH_ALLOCATORS
		nctl::swap(first.alloc_, second.alloc_);
#endif
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
	const T &front() const;
	/// Returns a reference to the first element in constant time
	T &front();
	/// Returns a constant reference to the last element in constant time
	const T &back() const;
	/// Returns a reference to the last element in constant time
	T &back();
	/// Inserts a new element as the first, in constant time
	inline void pushFront(const T &element) { insertBeforeNode(static_cast<ListNode<T> *>(sentinel_.next_), element); }
	/// Move inserts a new element as the first, in constant time
	inline void pushFront(T &&element) { insertBeforeNode(static_cast<ListNode<T> *>(sentinel_.next_), nctl::move(element)); }
	/// Constructs a new element as the first, in constant time
	template <typename... Args> void emplaceFront(Args &&... args) { emplaceBeforeNode(static_cast<ListNode<T> *>(sentinel_.next_), nctl::forward<Args>(args)...); }
	/// Inserts a new element as the last, in constant time
	inline void pushBack(const T &element) { insertAfterNode(static_cast<ListNode<T> *>(sentinel_.previous_), element); }
	/// Move inserts a new element as the last, in constant time
	inline void pushBack(T &&element) { insertAfterNode(static_cast<ListNode<T> *>(sentinel_.previous_), nctl::move(element)); }
	/// Constructs a new element as the last, in constant time
	template <typename... Args> void emplaceBack(Args &&... args) { emplaceAfterNode(static_cast<ListNode<T> *>(sentinel_.previous_), nctl::forward<Args>(args)...); }
	/// Removes the first element in constant time
	inline void popFront() { removeNode(sentinel_.next_); }
	/// Removes the last element in constant time
	inline void popBack() { removeNode(sentinel_.previous_); }
	/// Inserts a new element after the node pointed by the constant iterator
	ConstIterator insertAfter(const Iterator position, const T &element);
	/// Move inserts a new element after the node pointed by the constant iterator
	ConstIterator insertAfter(const Iterator position, T &&element);
	/// Constructs a new element after the node pointed by the constant iterator
	template <typename... Args> ConstIterator emplaceAfter(const Iterator position, Args &&... args);
	/// Inserts a new element before the node pointed by the constant iterator
	ConstIterator insertBefore(const Iterator position, const T &element);
	/// Move inserts a new element before the node pointed by the constant iterator
	ConstIterator insertBefore(const Iterator position, T &&element);
	/// Constructs a new element before the node pointed by the constant iterator
	template <typename... Args> ConstIterator emplaceBefore(const Iterator position, Args &&... args);
	/// Inserts new elements from a source range after the node pointed by the constant iterator, last not included
	ConstIterator insert(const Iterator position, Iterator first, const Iterator last);
	/// Removes the node pointed by the constant iterator in constant time
	ConstIterator erase(ConstIterator position);
	/// Removes the range of nodes pointed by the iterators in constant time
	ConstIterator erase(ConstIterator first, const ConstIterator last);
	/// Removes a specified element in linear time
	void remove(const T &element);
	/// Removes all the elements that fulfill the condition
	template <class Predicate>
	void removeIf(Predicate pred);
	/// Transfers all the elements from the source list in front of `position`
	void splice(Iterator position, List &source);
	/// Transfers one element at `it` from the source list in front of `position`
	void splice(Iterator position, List &source, Iterator it);
	/// Transfers a range of elements from the source list, `last` not included, in front of `position`
	void splice(Iterator position, List &source, Iterator first, Iterator last);

  private:
#if NCINE_WITH_ALLOCATORS
	/// The custom memory allocator for the list
	IAllocator &alloc_;
#endif
	/// Number of elements in the list
	unsigned int size_;
	/// The sentinel node
	BaseListNode sentinel_;

	/// Inserts a new element after a specified node
	ListNode<T> *insertAfterNode(ListNode<T> *node, const T &element);
	/// Move inserts a new element after a specified node
	ListNode<T> *insertAfterNode(ListNode<T> *node, T &&element);
	/// Constructs a new element after a specified node
	template <typename... Args> ListNode<T> *emplaceAfterNode(ListNode<T> *node, Args &&... args);
	/// Inserts a new element before a specified node
	ListNode<T> *insertBeforeNode(ListNode<T> *node, const T &element);
	/// Move inserts a new element before a specified node
	ListNode<T> *insertBeforeNode(ListNode<T> *node, T &&element);
	/// Constructs a new element before a specified node
	template <typename... Args> ListNode<T> *emplaceBeforeNode(ListNode<T> *node, Args &&... args);
	/// Removes a specified node in constant time
	ListNode<T> *removeNode(BaseListNode *node);
	/// Removes a range of nodes in constant time, last not included
	ListNode<T> *removeRange(ListNode<T> *firstNode, ListNode<T> *lastNode);
};

template <class T>
List<T>::List(const List<T> &other)
    :
#if NCINE_WITH_ALLOCATORS
      alloc_(other.alloc_),
#endif
      size_(0)
{
	for (List<T>::ConstIterator i = other.begin(); i != other.end(); ++i)
		pushBack(*i);
}

template <class T>
List<T>::List(List<T> &&other)
    :
#if NCINE_WITH_ALLOCATORS
      alloc_(other.alloc_),
#endif
      size_(other.size_)
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

template <class T>
List<T> &List<T>::operator=(const List<T> &other)
{
	if (this != &other)
	{
		clear();
		for (List<T>::ConstIterator i = other.begin(); i != other.end(); ++i)
			pushBack(*i);
	}

	return *this;
}

template <class T>
List<T> &List<T>::operator=(List<T> &&other)
{
	if (this == &other)
		return *this;

	clear();
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
#if !NCINE_WITH_ALLOCATORS
		delete static_cast<ListNode<T> *>(sentinel_.next_);
#else
		alloc_.deleteObject(static_cast<ListNode<T> *>(sentinel_.next_));
#endif
		sentinel_.next_ = nextNode;
	}

	sentinel_.previous_ = &sentinel_;
	size_ = 0;
}

template <class T>
const T &List<T>::front() const
{
	ASSERT(sentinel_.next_ != &sentinel_);
	return static_cast<ListNode<T> *>(sentinel_.next_)->data_;
}

template <class T>
T &List<T>::front()
{
	ASSERT(sentinel_.next_ != &sentinel_);
	return static_cast<ListNode<T> *>(sentinel_.next_)->data_;
}

template <class T>
const T &List<T>::back() const
{
	ASSERT(sentinel_.previous_ != &sentinel_);
	return static_cast<ListNode<T> *>(sentinel_.previous_)->data_;
}

template <class T>
T &List<T>::back()
{
	ASSERT(sentinel_.previous_ != &sentinel_);
	return static_cast<ListNode<T> *>(sentinel_.previous_)->data_;
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
template <typename... Args>
inline typename List<T>::ConstIterator List<T>::emplaceAfter(const Iterator position, Args &&... args)
{
	return ConstIterator(emplaceAfterNode(position.node_, nctl::forward<Args>(args)...));
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
template <typename... Args>
inline typename List<T>::ConstIterator List<T>::emplaceBefore(const Iterator position, Args &&... args)
{
	return ConstIterator(emplaceBeforeNode(position.node_, nctl::forward<Args>(args)...));
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

template <class T>
template <class Predicate>
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
#if !NCINE_WITH_ALLOCATORS
	ListNode<T> *newNode = new ListNode<T>(node, node->next_, element);
#else
	ListNode<T> *newNode = alloc_.newObject<ListNode<T>>(node, node->next_, element);
#endif

	// it also works if `node->next_` is the sentinel
	node->next_->previous_ = newNode;
	node->next_ = newNode;
	size_++;

	return newNode;
}

template <class T>
ListNode<T> *List<T>::insertAfterNode(ListNode<T> *node, T &&element)
{
#if !NCINE_WITH_ALLOCATORS
	ListNode<T> *newNode = new ListNode<T>(node, node->next_, nctl::move(element));
#else
	ListNode<T> *newNode = alloc_.newObject<ListNode<T>>(node, node->next_, nctl::move(element));
#endif

	// it also works if `node->next_` is the sentinel
	node->next_->previous_ = newNode;
	node->next_ = newNode;
	size_++;

	return newNode;
}

template <class T>
template <typename... Args>
ListNode<T> *List<T>::emplaceAfterNode(ListNode<T> *node, Args &&... args)
{
#if !NCINE_WITH_ALLOCATORS
	ListNode<T> *newNode = new ListNode<T>(node, node->next_, nctl::forward<Args>(args)...);
#else
	ListNode<T> *newNode = alloc_.newObject<ListNode<T>>(node, node->next_, nctl::forward<Args>(args)...);
#endif

	// it also works if `node->next_` is the sentinel
	node->next_->previous_ = newNode;
	node->next_ = newNode;
	size_++;

	return newNode;
}

template <class T>
ListNode<T> *List<T>::insertBeforeNode(ListNode<T> *node, const T &element)
{
#if !NCINE_WITH_ALLOCATORS
	ListNode<T> *newNode = new ListNode<T>(node->previous_, node, element);
#else
	ListNode<T> *newNode = alloc_.newObject<ListNode<T>>(node->previous_, node, element);
#endif

	// it also works if `node->previous_` is the sentinel
	node->previous_->next_ = newNode;
	node->previous_ = newNode;
	size_++;

	return newNode;
}

template <class T>
ListNode<T> *List<T>::insertBeforeNode(ListNode<T> *node, T &&element)
{
#if !NCINE_WITH_ALLOCATORS
	ListNode<T> *newNode = new ListNode<T>(node->previous_, node, nctl::move(element));
#else
	ListNode<T> *newNode = alloc_.newObject<ListNode<T>>(node->previous_, node, nctl::move(element));
#endif

	// it also works if `node->previous_` is the sentinel
	node->previous_->next_ = newNode;
	node->previous_ = newNode;
	size_++;

	return newNode;
}

template <class T>
template <typename... Args>
ListNode<T> *List<T>::emplaceBeforeNode(ListNode<T> *node, Args &&... args)
{
#if !NCINE_WITH_ALLOCATORS
	ListNode<T> *newNode = new ListNode<T>(node->previous_, node, nctl::forward<Args>(args)...);
#else
	ListNode<T> *newNode = alloc_.newObject<ListNode<T>>(node->previous_, node, nctl::forward<Args>(args)...);

#endif

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
#if !NCINE_WITH_ALLOCATORS
		delete static_cast<ListNode<T> *>(current);
#else
		alloc_.deleteObject(static_cast<ListNode<T> *>(current));
#endif
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
