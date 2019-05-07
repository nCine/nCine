#ifndef CLASS_NCTL_STATICHASHSETITERATOR
#define CLASS_NCTL_STATICHASHSETITERATOR

#include "StaticHashSet.h"
#include "iterator.h"

namespace nctl {

/// Base helper structure for type traits used in the hashset iterator
template <class K, class HashFunc, unsigned int Capacity>
struct StaticHashSetHelperTraits
{
	using HashSetPtr = const StaticHashSet<K, Capacity, HashFunc> *;
};

/// A hashset iterator
template <class K, class HashFunc, unsigned int Capacity>
class StaticHashSetIterator
{
  public:
	/// Reference type which respects iterator constness
	using Reference = typename IteratorTraits<StaticHashSetIterator>::Reference;

	/// Sentinel tags to initialize the iterator at the beginning and end
	enum class SentinelTagInit
	{
		/// Iterator at the beginning, next element is the first one
		BEGINNING,
		/// Iterator at the end, previous element is the last one
		END
	};

	StaticHashSetIterator(typename StaticHashSetHelperTraits<K, HashFunc, Capacity>::HashSetPtr hashSet, unsigned int bucketIndex)
	    : hashSet_(hashSet), bucketIndex_(bucketIndex), tag_(SentinelTag::REGULAR) {}

	StaticHashSetIterator(typename StaticHashSetHelperTraits<K, HashFunc, Capacity>::HashSetPtr hashSet, SentinelTagInit tag);

	/// Deferencing operator
	Reference operator*() const;

	/// Iterates to the next element (prefix)
	StaticHashSetIterator &operator++();
	/// Iterates to the next element (postfix)
	StaticHashSetIterator operator++(int);

	/// Iterates to the previous element (prefix)
	StaticHashSetIterator &operator--();
	/// Iterates to the previous element (postfix)
	StaticHashSetIterator operator--(int);

	/// Equality operator
	friend inline bool operator==(const StaticHashSetIterator &lhs, const StaticHashSetIterator &rhs)
	{
		if (lhs.tag_ == SentinelTag::REGULAR && rhs.tag_ == SentinelTag::REGULAR)
			return (lhs.hashSet_ == rhs.hashSet_ && lhs.bucketIndex_ == rhs.bucketIndex_);
		else
			return (lhs.tag_ == rhs.tag_);
	}

	/// Inequality operator
	friend inline bool operator!=(const StaticHashSetIterator &lhs, const StaticHashSetIterator &rhs)
	{
		if (lhs.tag_ == SentinelTag::REGULAR && rhs.tag_ == SentinelTag::REGULAR)
			return (lhs.hashSet_ != rhs.hashSet_ || lhs.bucketIndex_ != rhs.bucketIndex_);
		else
			return (lhs.tag_ != rhs.tag_);
	}

	/// Returns the key associated to the currently pointed node
	const K &key() const;
	/// Returns the hash associated to the currently pointed node
	hash_t hash() const;

  private:
	/// Sentinel tags to detect begin and end conditions
	enum SentinelTag
	{
		/// Iterator poiting to a real element
		REGULAR,
		/// Iterator at the beginning, next element is the first one
		BEGINNING,
		/// Iterator at the end, previous element is the last one
		END
	};

	typename StaticHashSetHelperTraits<K, HashFunc, Capacity>::HashSetPtr hashSet_;
	unsigned int bucketIndex_;
	SentinelTag tag_;

	/// Makes the iterator point to the next element in the hashset
	void next();
	/// Makes the iterator point to the previous element in the hashset
	void previous();
};

/// Iterator traits structure specialization for `HashSetIterator` class
template <class K, class HashFunc, unsigned int Capacity>
struct IteratorTraits<StaticHashSetIterator<K, HashFunc, Capacity>>
{
	/// Type of the values deferenced by the iterator (never const)
	using ValueType = K;
	/// Pointer to the type of the values deferenced by the iterator
	using Pointer = const K *;
	/// Reference to the type of the values deferenced by the iterator
	using Reference = const K &;
	/// Type trait for iterator category
	static inline BidirectionalIteratorTag IteratorCategory() { return BidirectionalIteratorTag(); }
};

template <class K, class HashFunc, unsigned int Capacity>
StaticHashSetIterator<K, HashFunc, Capacity>::StaticHashSetIterator(typename StaticHashSetHelperTraits<K, HashFunc, Capacity>::HashSetPtr hashSet, SentinelTagInit tag)
    : hashSet_(hashSet), bucketIndex_(0)
{
	switch (tag)
	{
		case SentinelTagInit::BEGINNING: tag_ = SentinelTag::BEGINNING; break;
		case SentinelTagInit::END: tag_ = SentinelTag::END; break;
	}
}

template <class K, class HashFunc, unsigned int Capacity>
typename StaticHashSetIterator<K, HashFunc, Capacity>::Reference StaticHashSetIterator<K, HashFunc, Capacity>::operator*() const
{
	return hashSet_->keys_[bucketIndex_];
}

template <class K, class HashFunc, unsigned int Capacity>
StaticHashSetIterator<K, HashFunc, Capacity> &StaticHashSetIterator<K, HashFunc, Capacity>::operator++()
{
	next();
	return *this;
}

template <class K, class HashFunc, unsigned int Capacity>
StaticHashSetIterator<K, HashFunc, Capacity> StaticHashSetIterator<K, HashFunc, Capacity>::operator++(int)
{
	// Create an unmodified copy to return
	StaticHashSetIterator<K, HashFunc, Capacity> iterator = *this;
	next();
	return iterator;
}

template <class K, class HashFunc, unsigned int Capacity>
StaticHashSetIterator<K, HashFunc, Capacity> &StaticHashSetIterator<K, HashFunc, Capacity>::operator--()
{
	previous();
	return *this;
}

template <class K, class HashFunc, unsigned int Capacity>
StaticHashSetIterator<K, HashFunc, Capacity> StaticHashSetIterator<K, HashFunc, Capacity>::operator--(int)
{
	// Create an unmodified copy to return
	StaticHashSetIterator<K, HashFunc, Capacity> iterator = *this;
	previous();
	return iterator;
}

template <class K, class HashFunc, unsigned int Capacity>
const K &StaticHashSetIterator<K, HashFunc, Capacity>::key() const
{
	return hashSet_->keys_[bucketIndex_];
}

template <class K, class HashFunc, unsigned int Capacity>
hash_t StaticHashSetIterator<K, HashFunc, Capacity>::hash() const
{
	return hashSet_->hashes_[bucketIndex_];
}

template <class K, class HashFunc, unsigned int Capacity>
void StaticHashSetIterator<K, HashFunc, Capacity>::next()
{
	if (tag_ == SentinelTag::REGULAR)
	{
		if (bucketIndex_ >= hashSet_->capacity() - 1)
		{
			tag_ = SentinelTag::END;
			return;
		}
		else
			bucketIndex_++;
	}
	else if (tag_ == SentinelTag::BEGINNING)
	{
		tag_ = SentinelTag::REGULAR;
		bucketIndex_ = 0;
	}
	else if (tag_ == SentinelTag::END)
		return;

	// Search the first non empty index starting from the current one
	while (bucketIndex_ < hashSet_->capacity() - 1 && hashSet_->hashes_[bucketIndex_] == NullHash)
		bucketIndex_++;

	if (hashSet_->hashes_[bucketIndex_] == NullHash)
		tag_ = SentinelTag::END;
}

template <class K, class HashFunc, unsigned int Capacity>
void StaticHashSetIterator<K, HashFunc, Capacity>::previous()
{
	if (tag_ == SentinelTag::REGULAR)
	{
		if (bucketIndex_ == 0)
		{
			tag_ = SentinelTag::BEGINNING;
			return;
		}
		else
			bucketIndex_--;
	}
	else if (tag_ == SentinelTag::END)
	{
		tag_ = SentinelTag::REGULAR;
		bucketIndex_ = hashSet_->capacity() - 1;
	}
	else if (tag_ == SentinelTag::BEGINNING)
		return;

	// Search the first non empty index starting from the current one
	while (bucketIndex_ > 0 && hashSet_->hashes_[bucketIndex_] == NullHash)
		bucketIndex_--;

	if (hashSet_->hashes_[bucketIndex_] == NullHash)
		tag_ = SentinelTag::BEGINNING;
}

}

#endif
