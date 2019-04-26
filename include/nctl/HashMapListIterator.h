#ifndef CLASS_NCTL_HASHMAPLISTITERATOR
#define CLASS_NCTL_HASHMAPLISTITERATOR

#include "HashMapList.h"
#include "iterator.h"

namespace nctl {

/// Base helper structure for type traits used in the hashmap iterator
template <class K, class T, class HashFunc, bool IsConst>
struct HashMapListHelperTraits
{};

/// Helper structure providing type traits used in the non constant hashmap iterator
template <class K, class T, class HashFunc>
struct HashMapListHelperTraits<K, T, HashFunc, false>
{
	using NodeType = typename HashMapList<K, T, HashFunc>::Node;
	using HashMapListPtr = HashMapList<K, T, HashFunc> *;
	using BucketPtr = typename HashMapList<K, T, HashFunc>::HashBucket *;
	using ListIteratorType = typename List<NodeType>::Iterator;
	using NodeReference = NodeType &;
};

/// Helper structure providing type traits used in the constant hashmap iterator
template <class K, class T, class HashFunc>
struct HashMapListHelperTraits<K, T, HashFunc, true>
{
	using NodeType = typename HashMapList<K, T, HashFunc>::Node;
	using HashMapListPtr = const HashMapList<K, T, HashFunc> *;
	using BucketPtr = const typename HashMapList<K, T, HashFunc>::HashBucket *;
	using ListIteratorType = typename List<NodeType>::ConstIterator;
	using NodeReference = const NodeType &;
};

/// A hashmap iterator
template <class K, class T, class HashFunc, bool IsConst>
class HashMapListIterator
{
  public:
	/// Reference type which respects iterator constness
	using Reference = typename IteratorTraits<HashMapListIterator>::Reference;

	/// Sentinel tags to initialize the iterator at the beginning and end
	enum class SentinelTagInit
	{
		/// Iterator at the beginning, next element is the first one
		BEGINNING,
		/// Iterator at the end, previous element is the last one
		END
	};

	HashMapListIterator(typename HashMapListHelperTraits<K, T, HashFunc, IsConst>::HashMapListPtr hashMap, unsigned int bucketIndex,
	                    typename HashMapListHelperTraits<K, T, HashFunc, IsConst>::ListIteratorType listIterator, bool atFirstNode)
	    : hashMap_(hashMap), bucketIndex_(bucketIndex), listIterator_(listIterator), atFirstNode_(atFirstNode), tag_(SentinelTag::REGULAR) {}

	HashMapListIterator(typename HashMapListHelperTraits<K, T, HashFunc, IsConst>::HashMapListPtr hashMap, SentinelTagInit tag);

	/// Copy constructor to implicitly convert a non constant iterator to a constant one
	HashMapListIterator(const HashMapListIterator<K, T, HashFunc, false> &it)
	    : hashMap_(it.hashMap_), bucketIndex_(it.bucketIndex_), listIterator_(it.listIterator_), atFirstNode_(it.atFirstNode_), tag_(SentinelTag(it.tag_)) {}

	/// Deferencing operator
	Reference operator*() const;

	/// Iterates to the next element (prefix)
	HashMapListIterator &operator++();
	/// Iterates to the next element (postfix)
	HashMapListIterator operator++(int);

	/// Iterates to the previous element (prefix)
	HashMapListIterator &operator--();
	/// Iterates to the previous element (postfix)
	HashMapListIterator operator--(int);

	/// Equality operator
	friend inline bool operator==(const HashMapListIterator &lhs, const HashMapListIterator &rhs)
	{
		if (lhs.tag_ == SentinelTag::REGULAR && rhs.tag_ == SentinelTag::REGULAR)
		{
			return (lhs.hashMap_ == rhs.hashMap_ && lhs.bucketIndex_ == rhs.bucketIndex_ &&
			        lhs.listIterator_ == rhs.listIterator_ && lhs.atFirstNode_ == rhs.atFirstNode_);
		}
		else
			return (lhs.tag_ == rhs.tag_);
	}

	/// Inequality operator
	friend inline bool operator!=(const HashMapListIterator &lhs, const HashMapListIterator &rhs)
	{
		if (lhs.tag_ == SentinelTag::REGULAR && rhs.tag_ == SentinelTag::REGULAR)
		{
			return (lhs.hashMap_ != rhs.hashMap_ || lhs.bucketIndex_ != rhs.bucketIndex_ ||
			        lhs.listIterator_ != rhs.listIterator_ || lhs.atFirstNode_ != rhs.atFirstNode_);
		}
		else
			return (lhs.tag_ != rhs.tag_);
	}

	/// Returns the hashmap node currently pointed by the iterator
	typename HashMapListHelperTraits<K, T, HashFunc, IsConst>::NodeReference node() const;
	/// Returns the value associated to the currently pointed node
	const T &value() const;
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

	typename HashMapListHelperTraits<K, T, HashFunc, IsConst>::HashMapListPtr hashMap_;
	unsigned int bucketIndex_;
	typename HashMapListHelperTraits<K, T, HashFunc, IsConst>::ListIteratorType listIterator_;
	bool atFirstNode_;
	SentinelTag tag_;

	/// Makes the iterator point to the next element in the hashmap
	void next();
	/// Makes the iterator point to the previous element in the hashmap
	void previous();
	/// Find the first valid element in the hashmap
	void findFirstValid();
	/// Find the last valid element in the hashmap
	void findLastValid();

	/// For non constant to constant iterator implicit conversion
	friend class HashMapListIterator<K, T, HashFunc, true>;
};

/// Iterator traits structure specialization for `HashMapListIterator` class
template <class K, class T, class HashFunc>
struct IteratorTraits<HashMapListIterator<K, T, HashFunc, false>>
{
	/// Type of the values deferenced by the iterator
	using ValueType = T;
	/// Pointer to the type of the values deferenced by the iterator
	using Pointer = T *;
	/// Reference to the type of the values deferenced by the iterator
	using Reference = T &;
	/// Type trait for iterator category
	static inline BidirectionalIteratorTag IteratorCategory() { return BidirectionalIteratorTag(); }
};

/// Iterator traits structure specialization for constant `HashMapListIterator` class
template <class K, class T, class HashFunc>
struct IteratorTraits<HashMapListIterator<K, T, HashFunc, true>>
{
	/// Type of the values deferenced by the iterator (never const)
	using ValueType = T;
	/// Pointer to the type of the values deferenced by the iterator
	using Pointer = const T *;
	/// Reference to the type of the values deferenced by the iterator
	using Reference = const T &;
	/// Type trait for iterator category
	static inline BidirectionalIteratorTag IteratorCategory() { return BidirectionalIteratorTag(); }
};

template <class K, class T, class HashFunc, bool IsConst>
HashMapListIterator<K, T, HashFunc, IsConst>::HashMapListIterator(typename HashMapListHelperTraits<K, T, HashFunc, IsConst>::HashMapListPtr hashMap, SentinelTagInit tag)
    : hashMap_(hashMap), bucketIndex_(0), listIterator_(nullptr), atFirstNode_(true)
{
	switch (tag)
	{
		case SentinelTagInit::BEGINNING: tag_ = SentinelTag::BEGINNING; break;
		case SentinelTagInit::END: tag_ = SentinelTag::END; break;
	}
}

template <class K, class T, class HashFunc, bool IsConst>
typename HashMapListIterator<K, T, HashFunc, IsConst>::Reference HashMapListIterator<K, T, HashFunc, IsConst>::operator*() const
{
	return node().value;
}

template <class K, class T, class HashFunc, bool IsConst>
HashMapListIterator<K, T, HashFunc, IsConst> &HashMapListIterator<K, T, HashFunc, IsConst>::operator++()
{
	next();
	return *this;
}

template <class K, class T, class HashFunc, bool IsConst>
HashMapListIterator<K, T, HashFunc, IsConst> HashMapListIterator<K, T, HashFunc, IsConst>::operator++(int)
{
	// Create an unmodified copy to return
	HashMapListIterator<K, T, HashFunc, IsConst> iterator = *this;
	next();
	return iterator;
}

template <class K, class T, class HashFunc, bool IsConst>
HashMapListIterator<K, T, HashFunc, IsConst> &HashMapListIterator<K, T, HashFunc, IsConst>::operator--()
{
	previous();
	return *this;
}

template <class K, class T, class HashFunc, bool IsConst>
HashMapListIterator<K, T, HashFunc, IsConst> HashMapListIterator<K, T, HashFunc, IsConst>::operator--(int)
{
	// Create an unmodified copy to return
	HashMapListIterator<K, T, HashFunc, IsConst> iterator = *this;
	previous();
	return iterator;
}

template <class K, class T, class HashFunc, bool IsConst>
typename HashMapListHelperTraits<K, T, HashFunc, IsConst>::NodeReference HashMapListIterator<K, T, HashFunc, IsConst>::node() const
{
	if (atFirstNode_)
		return hashMap_->buckets_[bucketIndex_].firstNode_;
	else
		return *listIterator_;
}

template <class K, class T, class HashFunc, bool IsConst>
const T &HashMapListIterator<K, T, HashFunc, IsConst>::value() const
{
	return node().value;
}

template <class K, class T, class HashFunc, bool IsConst>
const K &HashMapListIterator<K, T, HashFunc, IsConst>::key() const
{
	return node().key;
}

template <class K, class T, class HashFunc, bool IsConst>
hash_t HashMapListIterator<K, T, HashFunc, IsConst>::hash() const
{
	return node().hash;
}

template <class K, class T, class HashFunc, bool IsConst>
void HashMapListIterator<K, T, HashFunc, IsConst>::next()
{
	if (tag_ == SentinelTag::BEGINNING)
	{
		findFirstValid();
		return;
	}
	else if (tag_ == SentinelTag::END)
		return;

	typename HashMapListHelperTraits<K, T, HashFunc, IsConst>::BucketPtr bucket = &(hashMap_->buckets_[bucketIndex_]);

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
		if (bucketIndex_ == hashMap_->buckets_.size() - 1)
			tag_ = SentinelTag::END;
		else
		{
			do
			{
				bucket = &(hashMap_->buckets_[++bucketIndex_]);
			} while (bucketIndex_ < hashMap_->buckets_.size() - 1 && bucket->size() == 0);
		}

		if (bucket->size() == 0)
			tag_ = SentinelTag::END;
		else
			atFirstNode_ = true;
	}
}

template <class K, class T, class HashFunc, bool IsConst>
void HashMapListIterator<K, T, HashFunc, IsConst>::previous()
{
	if (tag_ == SentinelTag::END)
	{
		findLastValid();
		return;
	}
	else if (tag_ == SentinelTag::BEGINNING)
		return;

	typename HashMapListHelperTraits<K, T, HashFunc, IsConst>::BucketPtr bucket = &(hashMap_->buckets_[bucketIndex_]);

	if (atFirstNode_)
	{
		if (bucketIndex_ == 0)
			tag_ = SentinelTag::BEGINNING;
		else
		{
			do
			{
				bucket = &(hashMap_->buckets_[--bucketIndex_]);
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

template <class K, class T, class HashFunc, bool IsConst>
void HashMapListIterator<K, T, HashFunc, IsConst>::findFirstValid()
{
	tag_ = SentinelTag::REGULAR;
	bucketIndex_ = 0;
	typename HashMapListHelperTraits<K, T, HashFunc, IsConst>::BucketPtr bucket = &(hashMap_->buckets_[bucketIndex_]);

	// Search the first non empty bucket starting from the first one
	while (bucketIndex_ < hashMap_->buckets_.size() - 1 && bucket->size() == 0)
		bucket = &(hashMap_->buckets_[++bucketIndex_]);

	if (bucket->size() == 0)
		tag_ = SentinelTag::END;
}

template <class K, class T, class HashFunc, bool IsConst>
void HashMapListIterator<K, T, HashFunc, IsConst>::findLastValid()
{
	tag_ = SentinelTag::REGULAR;
	bucketIndex_ = hashMap_->buckets_.size() - 1;
	typename HashMapListHelperTraits<K, T, HashFunc, IsConst>::BucketPtr bucket = &(hashMap_->buckets_[bucketIndex_]);

	// Search the first non empty bucket starting from the last one
	while (bucketIndex_ > 0 && bucket->size() == 0)
		bucket = &(hashMap_->buckets_[--bucketIndex_]);

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
