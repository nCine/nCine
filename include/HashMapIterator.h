#ifndef CLASS_NCINE_HASHMAPITERATOR
#define CLASS_NCINE_HASHMAPITERATOR

#include "HashMap.h"
#include "List.h"
#include "iterator_traits.h"

namespace ncine {

template <class K, class T, class HashFunc, bool IsConst>
struct HelperTraits { };

template <class K, class T, class HashFunc>
struct HelperTraits<K, T, HashFunc, false>
{
	typedef HashMap<K, T, HashFunc>* HashMapPtr;
	typedef typename HashMap<K, T, HashFunc>::HashBucket* BucketPtr;
	typedef typename List<HashMapNode<K, T> >::Iterator ListIteratorType;
	typedef HashMapNode<K, T>& NodeReference;
};

template <class K, class T, class HashFunc>
struct HelperTraits<K, T, HashFunc, true>
{
	typedef const HashMap<K, T, HashFunc>* HashMapPtr;
	typedef const typename HashMap<K, T, HashFunc>::HashBucket* BucketPtr;
	typedef typename List<HashMapNode<K, T> >::ConstIterator ListIteratorType;
	typedef const HashMapNode<K, T>& NodeReference;
};

/// A hashmap iterator
template <class K, class T, class HashFunc, bool IsConst>
class HashMapIterator
{
  public:
	/// Reference type which respects iterator constness
	typedef typename IteratorTraits<HashMapIterator>::Reference Reference;

	HashMapIterator(typename HelperTraits<K, T, HashFunc, IsConst>::HashMapPtr hashMap, int bucketIndex,
					typename HelperTraits<K, T, HashFunc, IsConst>::ListIteratorType listIterator, bool atFirstNode)
		: hashMap_(hashMap), bucketIndex_(bucketIndex), listIterator_(listIterator), atFirstNode_(atFirstNode) { }

	/// Copy constructor to implicitly convert a non constant iterator to a constant one
	HashMapIterator(const HashMapIterator<K, T, HashFunc, false>& it)
		: hashMap_(it.hashMap_), bucketIndex_(it.bucketIndex_), listIterator_(it.listIterator_), atFirstNode_(it.atFirstNode_) { }

	// Deferencing operator
	Reference operator*() const;

	// Iterates to the next element (prefix)
	HashMapIterator& operator++();
	// Iterates to the next element (postfix)
	HashMapIterator operator++(int);

	// Iterates to the previous element (prefix)
	HashMapIterator& operator--();
	// Iterates to the previous element (postfix)
	HashMapIterator operator--(int);

	/// Equality operator
	bool operator==(const HashMapIterator& iterator) const;
	/// Inequality operator
	bool operator!=(const HashMapIterator& iterator) const;

	// Returns the hashmap node currently pointed by the iterator
	typename HelperTraits<K, T, HashFunc, IsConst>::NodeReference node() const;
	// Returns the value associated to the currently pointed node
	const T& value() const;
	// Returns the key associated to the currently pointed node
	const K& key() const;
	// Returns the hash associated to the currently pointed node
	hash_t hash() const;

  private:
	typename HelperTraits<K, T, HashFunc, IsConst>::HashMapPtr hashMap_;
	unsigned int bucketIndex_;
	typename HelperTraits<K, T, HashFunc, IsConst>::ListIteratorType listIterator_;
	bool atFirstNode_;

	// Makes the iterator point to the next element in the hashmap
	void next();
	// Makes the iterator point to the previous element in the hashmap
	void previous();

	// For non constant to constant iterator implicit conversion
	friend class HashMapIterator<K, T, HashFunc, true>;
};

/// Iterator traits structure specialization
template <class K, class T, class HashFunc>
struct IteratorTraits<HashMapIterator<K, T, HashFunc, false> >
{
	/// Type of the values deferenced by the iterator
	typedef T ValueType;
	/// Pointer to the type of the values deferenced by the iterator
	typedef T* Pointer;
	/// Reference to the type of the values deferenced by the iterator
	typedef T& Reference;
	/// Type trait for iterator category
	static inline BidirectionalIteratorTag IteratorCategory() { return BidirectionalIteratorTag(); }
};

/// Constant iterator traits structure specialization
template <class K, class T, class HashFunc>
struct IteratorTraits<HashMapIterator<K, T, HashFunc, true> >
{
	/// Type of the values deferenced by the iterator (never const)
	typedef T ValueType;
	/// Pointer to the type of the values deferenced by the iterator
	typedef const T* Pointer;
	/// Reference to the type of the values deferenced by the iterator
	typedef const T& Reference;
	/// Type trait for iterator category
	static inline BidirectionalIteratorTag IteratorCategory() { return BidirectionalIteratorTag(); }
};

/// Deferencing operator
template <class K, class T, class HashFunc, bool IsConst>
inline typename HashMapIterator<K, T, HashFunc, IsConst>::Reference HashMapIterator<K, T, HashFunc, IsConst>::operator*() const
{
	return node().value;
}

/// Iterates to the next element (prefix)
template <class K, class T, class HashFunc, bool IsConst>
HashMapIterator<K, T, HashFunc, IsConst>& HashMapIterator<K, T, HashFunc, IsConst>::operator++()
{
	next();
	return *this;
}

/// Iterates to the next element (postfix)
template <class K, class T, class HashFunc, bool IsConst>
HashMapIterator<K, T, HashFunc, IsConst> HashMapIterator<K, T, HashFunc, IsConst>::operator++(int)
{
	// Create an unmodified copy to return
	HashMapIterator<K, T, HashFunc, IsConst> iterator = *this;
	next();
	return iterator;
}

/// Iterates to the previous element (prefix)
template <class K, class T, class HashFunc, bool IsConst>
HashMapIterator<K, T, HashFunc, IsConst>& HashMapIterator<K, T, HashFunc, IsConst>::operator--()
{
	previous();
	return *this;
}

/// Iterates to the previous element (postfix)
template <class K, class T, class HashFunc, bool IsConst>
HashMapIterator<K, T, HashFunc, IsConst> HashMapIterator<K, T, HashFunc, IsConst>::operator--(int)
{
	// Create an unmodified copy to return
	HashMapIterator<K, T, HashFunc, IsConst> iterator = *this;
	previous();
	return iterator;
}

/// Equality operator
template <class K, class T, class HashFunc, bool IsConst>
inline bool HashMapIterator<K, T, HashFunc, IsConst>::operator==(const HashMapIterator<K, T, HashFunc, IsConst>& iterator) const
{
	return (hashMap_ == iterator.hashMap_ && bucketIndex_ == iterator.bucketIndex_ &&
			listIterator_ == iterator.listIterator_ && atFirstNode_ == iterator.atFirstNode_);
}

/// Inequality operator
template <class K, class T, class HashFunc, bool IsConst>
inline bool HashMapIterator<K, T, HashFunc, IsConst>::operator!=(const HashMapIterator<K, T, HashFunc, IsConst>& iterator) const
{
	return (hashMap_ != iterator.hashMap_ || bucketIndex_ != iterator.bucketIndex_ ||
			listIterator_ != iterator.listIterator_ || atFirstNode_ != iterator.atFirstNode_);
}

/// Returns the hashmap node currently pointed by the iterator
template <class K, class T, class HashFunc, bool IsConst>
typename HelperTraits<K, T, HashFunc, IsConst>::NodeReference HashMapIterator<K, T, HashFunc, IsConst>::node() const
{
	if (atFirstNode_)
	{
		return hashMap_->buckets_[bucketIndex_].firstNode_;
	}
	else
	{
		return *listIterator_;
	}
}

/// Returns the value associated to the currently pointed node
template <class K, class T, class HashFunc, bool IsConst>
inline const T& HashMapIterator<K, T, HashFunc, IsConst>::value() const
{
	return node().value;
}

/// Returns the key associated to the currently pointed node
template <class K, class T, class HashFunc, bool IsConst>
inline const K& HashMapIterator<K, T, HashFunc, IsConst>::key() const
{
	return node().key;
}

/// Returns the hash associated to the currently pointed node
template <class K, class T, class HashFunc, bool IsConst>
inline hash_t HashMapIterator<K, T, HashFunc, IsConst>::hash() const
{
	return node().hash;
}

/// Makes the iterator point to the next element in the hashmap
template <class K, class T, class HashFunc, bool IsConst>
void HashMapIterator<K, T, HashFunc, IsConst>::next()
{
	typename HelperTraits<K, T, HashFunc, IsConst>::BucketPtr bucket = &(hashMap_->buckets_[bucketIndex_]);

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
	if (listIterator_ == bucket->collisionList_.end() && bucketIndex_ < hashMap_->buckets_.size()-1)
	{
		do
		{
			bucket = &(hashMap_->buckets_[++bucketIndex_]);
		} while(bucketIndex_ < hashMap_->buckets_.size()-1 && bucket->size() == 0);

		if (bucket->size() > 0)
		{
			atFirstNode_ = true;
		}
	}
}

/// Makes the iterator point to the previous element in the hashmap
template <class K, class T, class HashFunc, bool IsConst>
void HashMapIterator<K, T, HashFunc, IsConst>::previous()
{
	typename HelperTraits<K, T, HashFunc, IsConst>::BucketPtr bucket = &(hashMap_->buckets_[bucketIndex_]);

	if (atFirstNode_ && bucketIndex_ > 0)
	{
		do
		{
			bucket = &(hashMap_->buckets_[--bucketIndex_]);
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
		atFirstNode_ = true;
	}
}

}

#endif
