#ifndef CLASS_NCTL_HASHSETITERATOR
#define CLASS_NCTL_HASHSETITERATOR

#include "HashSet.h"
#include "iterator.h"

namespace nctl {

/// Base helper structure for type traits used in the hashset iterator
template <class K, class HashFunc>
struct HashSetHelperTraits
{
	using HashSetPtr = const HashSet<K, HashFunc> *;
};

/// A hashset iterator
template <class K, class HashFunc>
class HashSetIterator
{
  public:
	/// Reference type which respects iterator constness
	using Reference = typename IteratorTraits<HashSetIterator>::Reference;

	/// Sentinel tags to initialize the iterator at the beginning and end
	enum class SentinelTagInit
	{
		/// Iterator at the beginning, next element is the first one
		BEGINNING,
		/// Iterator at the end, previous element is the last one
		END
	};

	HashSetIterator(typename HashSetHelperTraits<K, HashFunc>::HashSetPtr hashSet, unsigned int bucketIndex)
	    : hashSet_(hashSet), bucketIndex_(bucketIndex), tag_(SentinelTag::REGULAR) {}

	HashSetIterator(typename HashSetHelperTraits<K, HashFunc>::HashSetPtr hashSet, SentinelTagInit tag);

	/// Deferencing operator
	Reference operator*() const;

	/// Iterates to the next element (prefix)
	HashSetIterator &operator++();
	/// Iterates to the next element (postfix)
	HashSetIterator operator++(int);

	/// Iterates to the previous element (prefix)
	HashSetIterator &operator--();
	/// Iterates to the previous element (postfix)
	HashSetIterator operator--(int);

	/// Equality operator
	friend inline bool operator==(const HashSetIterator &lhs, const HashSetIterator &rhs)
	{
		if (lhs.tag_ == SentinelTag::REGULAR && rhs.tag_ == SentinelTag::REGULAR)
			return (lhs.hashSet_ == rhs.hashSet_ && lhs.bucketIndex_ == rhs.bucketIndex_);
		else
			return (lhs.tag_ == rhs.tag_);
	}

	/// Inequality operator
	friend inline bool operator!=(const HashSetIterator &lhs, const HashSetIterator &rhs)
	{
		if (lhs.tag_ == SentinelTag::REGULAR && rhs.tag_ == SentinelTag::REGULAR)
			return (lhs.hashSet_ != rhs.hashSet_ || lhs.bucketIndex_ != rhs.bucketIndex_);
		else
			return (lhs.tag_ != rhs.tag_);
	}

	/// Returns the key associated to the currently pointed element
	const K &key() const;
	/// Returns the hash associated to the currently pointed element
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

	typename HashSetHelperTraits<K, HashFunc>::HashSetPtr hashSet_;
	unsigned int bucketIndex_;
	SentinelTag tag_;

	/// Makes the iterator point to the next element in the hashSet
	void next();
	/// Makes the iterator point to the previous element in the hashset
	void previous();
};

/// Iterator traits structure specialization for `HashSetIterator` class
template <class K, class HashFunc>
struct IteratorTraits<HashSetIterator<K, HashFunc>>
{
	/// Type of the values deferenced by the iterator (never const)
	using ValueType = K;
	/// Pointer to the type of the values deferenced by the iterator
	using Pointer = const K *;
	/// Reference to the type of the values deferenced by the iterator
	using Reference = const K &;
	/// Type trait for iterator category
	using IteratorCategory = BidirectionalIteratorTag;
};

template <class K, class HashFunc>
HashSetIterator<K, HashFunc>::HashSetIterator(typename HashSetHelperTraits<K, HashFunc>::HashSetPtr hashSet, SentinelTagInit tag)
    : hashSet_(hashSet), bucketIndex_(0)
{
	switch (tag)
	{
		case SentinelTagInit::BEGINNING: tag_ = SentinelTag::BEGINNING; break;
		case SentinelTagInit::END: tag_ = SentinelTag::END; break;
	}
}

template <class K, class HashFunc>
typename HashSetIterator<K, HashFunc>::Reference HashSetIterator<K, HashFunc>::operator*() const
{
	return hashSet_->keys_[bucketIndex_];
}

template <class K, class HashFunc>
HashSetIterator<K, HashFunc> &HashSetIterator<K, HashFunc>::operator++()
{
	next();
	return *this;
}

template <class K, class HashFunc>
HashSetIterator<K, HashFunc> HashSetIterator<K, HashFunc>::operator++(int)
{
	// Create an unmodified copy to return
	HashSetIterator<K, HashFunc> iterator = *this;
	next();
	return iterator;
}

template <class K, class HashFunc>
HashSetIterator<K, HashFunc> &HashSetIterator<K, HashFunc>::operator--()
{
	previous();
	return *this;
}

template <class K, class HashFunc>
HashSetIterator<K, HashFunc> HashSetIterator<K, HashFunc>::operator--(int)
{
	// Create an unmodified copy to return
	HashSetIterator<K, HashFunc> iterator = *this;
	previous();
	return iterator;
}

template <class K, class HashFunc>
const K &HashSetIterator<K, HashFunc>::key() const
{
	return hashSet_->keys_[bucketIndex_];
}

template <class K, class HashFunc>
hash_t HashSetIterator<K, HashFunc>::hash() const
{
	return hashSet_->hashes_[bucketIndex_];
}

template <class K, class HashFunc>
void HashSetIterator<K, HashFunc>::next()
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

template <class K, class HashFunc>
void HashSetIterator<K, HashFunc>::previous()
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
