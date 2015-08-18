#ifndef CLASS_NCINE_HASHMAPITERATOR
#define CLASS_NCINE_HASHMAPITERATOR

#include "HashMap.h"
#include "ListIterator.h"

namespace ncine {

/// A hashmap iterator
template <class K, class T, class HashFunc>
class HashMapIterator
{
  public:
	HashMapIterator(const HashMap<K, T, HashFunc>& hashMap, int bucketIndex,
					const ListIterator<HashMapNode<K, T> >& listIterator, const HashMapNode<K, T>& firstNode, bool atFirstNode)
		: hashMap_(hashMap), bucketIndex_(bucketIndex), listIterator_(listIterator),
		  firstNode_(firstNode), atFirstNode_(atFirstNode) { }
	HashMapIterator(const HashMapIterator& it)
		: hashMap_(it.hashMap_), bucketIndex_(it.bucketIndex_), listIterator_(it.listIterator_),
		  firstNode_(it.firstNode_), atFirstNode_(it.atFirstNode_) { }

	// Read-only deferencing operator
	const HashMapNode<K, T>& operator*() const;
	// Deferencing operator
	HashMapNode<K, T>& operator*();

	// Iterates to the next element (prefix)
	HashMapIterator<K, T, HashFunc> operator++() const;
	// Iterates to the next element (postfix)
	HashMapIterator<K, T, HashFunc> operator++(int) const;

	// Iterates to the previous element (prefix)
	HashMapIterator<K, T, HashFunc> operator--() const;
	// Iterates to the previous element (postfix)
	HashMapIterator<K, T, HashFunc> operator--(int) const;

	/// Equality operator
	bool operator==(const HashMapIterator<K, T, HashFunc>& iterator) const;
	/// Inequality operator
	bool operator!=(const HashMapIterator<K, T, HashFunc>& iterator) const;

	HashMapIterator begin(const HashMap<K, T, HashFunc>& hashMap);
	HashMapIterator end(const HashMap<K, T, HashFunc>& hashMap);

	const HashMapIterator begin(const HashMap<K, T, HashFunc>& hashMap) const;
	const HashMapIterator end(const HashMap<K, T, HashFunc>& hashMap) const;

  private:
	const HashMap<K, T, HashFunc>& hashMap_;
	mutable unsigned int bucketIndex_;
	mutable ListIterator<HashMapNode<K, T> > listIterator_;
	mutable HashMapNode<K, T> firstNode_;
	mutable bool atFirstNode_;

	// Makes the iterator point to the next element in the hashmap
	void next() const;
	// Makes the iterator point to the previous element in the hashmap
	void previous() const;
};

/// Read-only deferencing operator
template <class K, class T, class HashFunc>
inline const HashMapNode<K, T>& HashMapIterator<K, T, HashFunc>::operator*() const
{
	if (atFirstNode_)
	{
		return firstNode_;
	}
	else
	{
		return *listIterator_;
	}
}

/// Deferencing operator
template <class K, class T, class HashFunc>
inline HashMapNode<K, T>& HashMapIterator<K, T, HashFunc>::operator*()
{
	if (atFirstNode_)
	{
		return firstNode_;
	}
	else
	{
		return *listIterator_;
	}
}

/// Iterates to the next element (prefix)
template <class K, class T, class HashFunc>
HashMapIterator<K, T, HashFunc> HashMapIterator<K, T, HashFunc>::operator++() const
{
	next();
	return *this;
}

/// Iterates to the next element (postfix)
template <class K, class T, class HashFunc>
HashMapIterator<K, T, HashFunc> HashMapIterator<K, T, HashFunc>::operator++(int) const
{
	// Create an unmodified copy to return
	HashMapIterator<K, T, HashFunc> iterator = *this;
	next();
	return iterator;
}

/// Iterates to the previous element (prefix)
template <class K, class T, class HashFunc>
HashMapIterator<K, T, HashFunc> HashMapIterator<K, T, HashFunc>::operator--() const
{
	previous();
	return *this;
}

/// Iterates to the previous element (postfix)
template <class K, class T, class HashFunc>
HashMapIterator<K, T, HashFunc> HashMapIterator<K, T, HashFunc>::operator--(int) const
{
	// Create an unmodified copy to return
	HashMapIterator<K, T, HashFunc> iterator = *this;
	previous();
	return iterator;
}

/// Equality operator
template <class K, class T, class HashFunc>
inline bool HashMapIterator<K, T, HashFunc>::operator==(const HashMapIterator<K, T, HashFunc>& iterator) const
{
	return (&hashMap_ == &iterator.hashMap_ && bucketIndex_ == iterator.bucketIndex_ &&
			listIterator_ == iterator.listIterator_ && atFirstNode_ == iterator.atFirstNode_);
}

/// Inequality operator
template <class K, class T, class HashFunc>
inline bool HashMapIterator<K, T, HashFunc>::operator!=(const HashMapIterator<K, T, HashFunc>& iterator) const
{
	return (&hashMap_ != &iterator.hashMap_ || bucketIndex_ != iterator.bucketIndex_ ||
			listIterator_ != iterator.listIterator_ || atFirstNode_ != iterator.atFirstNode_);
}

template <class K, class T, class HashFunc>
HashMapIterator<K, T, HashFunc> HashMapIterator<K, T, HashFunc>::begin(const HashMap<K, T, HashFunc>& hashMap)
{
	HashMapIterator<K, T, HashFunc> iterator = HashMapIterator(hashMap, 0, hashMap.buckets_[0].collisionList_.begin(), hashMap.buckets_[0].firstNode_, true);
	if (hashMap.buckets_[0].size_ == 0)
	{
		iterator.next();
	}

	return iterator;
}

template <class K, class T, class HashFunc>
HashMapIterator<K, T, HashFunc> HashMapIterator<K, T, HashFunc>::end(const HashMap<K, T, HashFunc>& hashMap)
{
	unsigned int lastIndex = hashMap.buckets_.size() - 1;
	return HashMapIterator(hashMap, lastIndex, hashMap.buckets_[lastIndex].collisionList_.end(), hashMap.buckets_[lastIndex].firstNode_, false);
}

template <class K, class T, class HashFunc>
const HashMapIterator<K, T, HashFunc> HashMapIterator<K, T, HashFunc>::begin(const HashMap<K, T, HashFunc>& hashMap) const
{
	HashMapIterator<K, T, HashFunc> iterator = HashMapIterator(hashMap, 0, hashMap.buckets_[0].collisionList_.begin(), hashMap.buckets_[0].firstNode_, true);
	if (hashMap.buckets_[0].size_ == 0)
	{
		iterator.next();
	}

	return iterator;
}

template <class K, class T, class HashFunc>
const HashMapIterator<K, T, HashFunc> HashMapIterator<K, T, HashFunc>::end(const HashMap<K, T, HashFunc>& hashMap) const
{
	unsigned int lastIndex = hashMap.buckets_.size() - 1;
	return HashMapIterator(hashMap, lastIndex, hashMap.buckets_[lastIndex].collisionList_.end(), hashMap.buckets_[lastIndex].firstNode_, false);
}

/// Makes the iterator point to the next element in the hashmap
template <class K, class T, class HashFunc>
void HashMapIterator<K, T, HashFunc>::next() const
{
	const typename HashMap<K, T, HashFunc>::HashBucket* bucket = &(hashMap_.buckets_[bucketIndex_]);

	if (atFirstNode_)
	{
		atFirstNode_ = false;
		listIterator_ = bucket->collisionList_.begin();
	}
	else
	{
		listIterator_++;
	}

	// The list iterator condition also applies when it points to the beginning of an empty list
	if (listIterator_ == bucket->collisionList_.end() && bucketIndex_ < hashMap_.buckets_.size()-1)
	{
		do
		{
			bucket = &(hashMap_.buckets_[++bucketIndex_]);
		} while(bucketIndex_ < hashMap_.buckets_.size()-1 && bucket->size() == 0);

		if (bucket->size() > 0)
		{
			firstNode_ = bucket->firstNode_;
			atFirstNode_ = true;
		}
	}
}

/// Makes the iterator point to the previous element in the hashmap
template <class K, class T, class HashFunc>
void HashMapIterator<K, T, HashFunc>::previous() const
{
	const typename HashMap<K, T, HashFunc>::HashBucket* bucket = &(hashMap_.buckets_[bucketIndex_]);

	if (atFirstNode_ && bucketIndex_ > 0)
	{
		do
		{
			bucket = &(hashMap_.buckets_[--bucketIndex_]);
		} while(bucketIndex_ > 0 && bucket->size() == 0);

		if (bucket->size() > 0)
		{
			atFirstNode_ = false;
			listIterator_ = bucket->collisionList_.end();
		}
	}
	else
	{
		listIterator_--;
	}

	if (listIterator_ == bucket->collisionList_.begin())
	{
		firstNode_ = bucket->firstNode_;
		atFirstNode_ = true;
	}
}

/// A way to simulate C++11 alias templates
template <class T>
struct StringHashMapIterator
{
	typedef HashMapIterator<String, T, SaxHashFunc<nc::String> > type;
};

}

#endif
