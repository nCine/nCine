#ifndef CLASS_NCTL_HASHSETLISTITERATOR
#define CLASS_NCTL_HASHSETLISTITERATOR

#include "HashSetList.h"
#include "iterator.h"

namespace nctl {

/// Base helper structure for type traits used in the hashset iterator
template <class K, class HashFunc>
struct HashSetListHelperTraits
{
	using NodeType = typename HashSetList<K, HashFunc>::Node;
	using HashSetListPtr = const HashSetList<K, HashFunc> *;
	using BucketPtr = const typename HashSetList<K, HashFunc>::HashBucket *;
	using ListIteratorType = typename List<NodeType>::ConstIterator;
	using NodeReference = const NodeType &;
};

/// A hashset iterator
template <class K, class HashFunc>
class HashSetListIterator
{
  public:
	/// Reference type which respects iterator constness
	using Reference = typename IteratorTraits<HashSetListIterator>::Reference;

	/// Sentinel tags to initialize the iterator at the beginning and end
	enum class SentinelTagInit
	{
		/// Iterator at the beginning, next element is the first one
		BEGINNING,
		/// Iterator at the end, previous element is the last one
		END
	};

	HashSetListIterator(typename HashSetListHelperTraits<K, HashFunc>::HashSetListPtr hashSet, unsigned int bucketIndex,
	                    typename HashSetListHelperTraits<K, HashFunc>::ListIteratorType listIterator, bool atFirstNode)
	    : hashSet_(hashSet), bucketIndex_(bucketIndex), listIterator_(listIterator), atFirstNode_(atFirstNode), tag_(SentinelTag::REGULAR) {}

	HashSetListIterator(typename HashSetListHelperTraits<K, HashFunc>::HashSetListPtr hashSet, SentinelTagInit tag);

	/// Deferencing operator
	Reference operator*() const;

	/// Iterates to the next element (prefix)
	HashSetListIterator &operator++();
	/// Iterates to the next element (postfix)
	HashSetListIterator operator++(int);

	/// Iterates to the previous element (prefix)
	HashSetListIterator &operator--();
	/// Iterates to the previous element (postfix)
	HashSetListIterator operator--(int);

	/// Equality operator
	friend inline bool operator==(const HashSetListIterator &lhs, const HashSetListIterator &rhs)
	{
		if (lhs.tag_ == SentinelTag::REGULAR && rhs.tag_ == SentinelTag::REGULAR)
		{
			return (lhs.hashSet_ == rhs.hashSet_ && lhs.bucketIndex_ == rhs.bucketIndex_ &&
			        lhs.listIterator_ == rhs.listIterator_ && lhs.atFirstNode_ == rhs.atFirstNode_);
		}
		else
			return (lhs.tag_ == rhs.tag_);
	}

	/// Inequality operator
	friend inline bool operator!=(const HashSetListIterator &lhs, const HashSetListIterator &rhs)
	{
		if (lhs.tag_ == SentinelTag::REGULAR && rhs.tag_ == SentinelTag::REGULAR)
		{
			return (lhs.hashSet_ != rhs.hashSet_ || lhs.bucketIndex_ != rhs.bucketIndex_ ||
			        lhs.listIterator_ != rhs.listIterator_ || lhs.atFirstNode_ != rhs.atFirstNode_);
		}
		else
			return (lhs.tag_ != rhs.tag_);
	}

	/// Returns the hashset node currently pointed by the iterator
	typename HashSetListHelperTraits<K, HashFunc>::NodeReference node() const;
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

	typename HashSetListHelperTraits<K, HashFunc>::HashSetListPtr hashSet_;
	unsigned int bucketIndex_;
	typename HashSetListHelperTraits<K, HashFunc>::ListIteratorType listIterator_;
	bool atFirstNode_;
	SentinelTag tag_;

	/// Makes the iterator point to the next element in the hashset
	void next();
	/// Makes the iterator point to the previous element in the hashset
	void previous();
	/// Find the first valid element in the hashset
	void findFirstValid();
	/// Find the last valid element in the hashmset
	void findLastValid();
};

/// Iterator traits structure specialization for `HashSetListIterator` class
template <class K, class HashFunc>
struct IteratorTraits<HashSetListIterator<K, HashFunc>>
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
HashSetListIterator<K, HashFunc>::HashSetListIterator(typename HashSetListHelperTraits<K, HashFunc>::HashSetListPtr hashSet, SentinelTagInit tag)
    : hashSet_(hashSet), bucketIndex_(0), listIterator_(nullptr), atFirstNode_(true)
{
	switch (tag)
	{
		case SentinelTagInit::BEGINNING: tag_ = SentinelTag::BEGINNING; break;
		case SentinelTagInit::END: tag_ = SentinelTag::END; break;
	}
}

template <class K, class HashFunc>
typename HashSetListIterator<K, HashFunc>::Reference HashSetListIterator<K, HashFunc>::operator*() const
{
	return node().key;
}

template <class K, class HashFunc>
HashSetListIterator<K, HashFunc> &HashSetListIterator<K, HashFunc>::operator++()
{
	next();
	return *this;
}

template <class K, class HashFunc>
HashSetListIterator<K, HashFunc> HashSetListIterator<K, HashFunc>::operator++(int)
{
	// Create an unmodified copy to return
	HashSetListIterator<K, HashFunc> iterator = *this;
	next();
	return iterator;
}

template <class K, class HashFunc>
HashSetListIterator<K, HashFunc> &HashSetListIterator<K, HashFunc>::operator--()
{
	previous();
	return *this;
}

template <class K, class HashFunc>
HashSetListIterator<K, HashFunc> HashSetListIterator<K, HashFunc>::operator--(int)
{
	// Create an unmodified copy to return
	HashSetListIterator<K, HashFunc> iterator = *this;
	previous();
	return iterator;
}

template <class K, class HashFunc>
typename HashSetListHelperTraits<K, HashFunc>::NodeReference HashSetListIterator<K, HashFunc>::node() const
{
	if (atFirstNode_)
		return hashSet_->buckets_[bucketIndex_].firstNode_;
	else
		return *listIterator_;
}

template <class K, class HashFunc>
const K &HashSetListIterator<K, HashFunc>::key() const
{
	return node().key;
}

template <class K, class HashFunc>
hash_t HashSetListIterator<K, HashFunc>::hash() const
{
	return node().hash;
}

template <class K, class HashFunc>
void HashSetListIterator<K, HashFunc>::next()
{
	if (tag_ == SentinelTag::BEGINNING)
	{
		findFirstValid();
		return;
	}
	else if (tag_ == SentinelTag::END)
		return;

	typename HashSetListHelperTraits<K, HashFunc>::BucketPtr bucket = &(hashSet_->buckets_[bucketIndex_]);

	if (atFirstNode_)
	{
		atFirstNode_ = false;
		listIterator_ = bucket->collisionList_.begin();
	}
	else
		++listIterator_;

	// The list iterator condition also applies when it points to the beginning of an empty list
	if (listIterator_ == bucket->collisionList_.end())
	{
		if (bucketIndex_ == hashSet_->buckets_.size() - 1)
			tag_ = SentinelTag::END;
		else
		{
			do
			{
				bucket = &(hashSet_->buckets_[++bucketIndex_]);
			} while (bucketIndex_ < hashSet_->buckets_.size() - 1 && bucket->size() == 0);
		}

		if (bucket->size() == 0)
			tag_ = SentinelTag::END;
		else
			atFirstNode_ = true;
	}
}

template <class K, class HashFunc>
void HashSetListIterator<K, HashFunc>::previous()
{
	if (tag_ == SentinelTag::END)
	{
		findLastValid();
		return;
	}
	else if (tag_ == SentinelTag::BEGINNING)
		return;

	typename HashSetListHelperTraits<K, HashFunc>::BucketPtr bucket = &(hashSet_->buckets_[bucketIndex_]);

	if (atFirstNode_)
	{
		if (bucketIndex_ == 0)
			tag_ = SentinelTag::BEGINNING;
		else
		{
			do
			{
				bucket = &(hashSet_->buckets_[--bucketIndex_]);
			} while (bucketIndex_ > 0 && bucket->size() == 0);
		}

		if (bucket->size() == 0)
			tag_ = SentinelTag::BEGINNING;
		else if (bucket->size() > 1)
		{
			atFirstNode_ = false;
			listIterator_ = --bucket->collisionList_.end();
		}
	}
	else
		--listIterator_;

	if (listIterator_ == bucket->collisionList_.end()) // nullptr sentinel
		atFirstNode_ = true;
}

template <class K, class HashFunc>
void HashSetListIterator<K, HashFunc>::findFirstValid()
{
	tag_ = SentinelTag::REGULAR;
	bucketIndex_ = 0;
	typename HashSetListHelperTraits<K, HashFunc>::BucketPtr bucket = &(hashSet_->buckets_[bucketIndex_]);

	// Search the first non empty bucket starting from the first one
	while (bucketIndex_ < hashSet_->buckets_.size() - 1 && bucket->size() == 0)
		bucket = &(hashSet_->buckets_[++bucketIndex_]);

	if (bucket->size() == 0)
		tag_ = SentinelTag::END;
}

template <class K, class HashFunc>
void HashSetListIterator<K, HashFunc>::findLastValid()
{
	tag_ = SentinelTag::REGULAR;
	bucketIndex_ = hashSet_->buckets_.size() - 1;
	typename HashSetListHelperTraits<K, HashFunc>::BucketPtr bucket = &(hashSet_->buckets_[bucketIndex_]);

	// Search the first non empty bucket starting from the last one
	while (bucketIndex_ > 0 && bucket->size() == 0)
		bucket = &(hashSet_->buckets_[--bucketIndex_]);

	if (bucket->size() > 1)
	{
		listIterator_ = --bucket->collisionList_.end();
		atFirstNode_ = false;
	}
	else if (bucket->size() == 0)
		tag_ = SentinelTag::BEGINNING;
}

}

#endif
