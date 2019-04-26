#ifndef CLASS_NCTL_HASHMAPITERATOR
#define CLASS_NCTL_HASHMAPITERATOR

#include "HashMap.h"
#include "iterator.h"

namespace nctl {

/// Base helper structure for type traits used in the hashmap iterator
template <class K, class T, class HashFunc, bool IsConst>
struct HashMapHelperTraits
{};

/// Helper structure providing type traits used in the non constant hashmap iterator
template <class K, class T, class HashFunc>
struct HashMapHelperTraits<K, T, HashFunc, false>
{
	using HashMapPtr = HashMap<K, T, HashFunc> *;
	using NodeReference = typename HashMap<K, T, HashFunc>::Node &;
};

/// Helper structure providing type traits used in the constant hashmap iterator
template <class K, class T, class HashFunc>
struct HashMapHelperTraits<K, T, HashFunc, true>
{
	using HashMapPtr = const HashMap<K, T, HashFunc> *;
	using NodeReference = const typename HashMap<K, T, HashFunc>::Node &;
};

/// A hashmap iterator
template <class K, class T, class HashFunc, bool IsConst>
class HashMapIterator
{
  public:
	/// Reference type which respects iterator constness
	using Reference = typename IteratorTraits<HashMapIterator>::Reference;

	/// Sentinel tags to initialize the iterator at the beginning and end
	enum class SentinelTagInit
	{
		/// Iterator at the beginning, next element is the first one
		BEGINNING,
		/// Iterator at the end, previous element is the last one
		END
	};

	HashMapIterator(typename HashMapHelperTraits<K, T, HashFunc, IsConst>::HashMapPtr hashMap, unsigned int bucketIndex)
	    : hashMap_(hashMap), bucketIndex_(bucketIndex), tag_(SentinelTag::REGULAR) {}

	HashMapIterator(typename HashMapHelperTraits<K, T, HashFunc, IsConst>::HashMapPtr hashMap, SentinelTagInit tag);

	/// Copy constructor to implicitly convert a non constant iterator to a constant one
	HashMapIterator(const HashMapIterator<K, T, HashFunc, false> &it)
	    : hashMap_(it.hashMap_), bucketIndex_(it.bucketIndex_), tag_(SentinelTag(it.tag_)) {}

	/// Deferencing operator
	Reference operator*() const;

	/// Iterates to the next element (prefix)
	HashMapIterator &operator++();
	/// Iterates to the next element (postfix)
	HashMapIterator operator++(int);

	/// Iterates to the previous element (prefix)
	HashMapIterator &operator--();
	/// Iterates to the previous element (postfix)
	HashMapIterator operator--(int);

	/// Equality operator
	friend inline bool operator==(const HashMapIterator &lhs, const HashMapIterator &rhs)
	{
		if (lhs.tag_ == SentinelTag::REGULAR && rhs.tag_ == SentinelTag::REGULAR)
			return (lhs.hashMap_ == rhs.hashMap_ && lhs.bucketIndex_ == rhs.bucketIndex_);
		else
			return (lhs.tag_ == rhs.tag_);
	}

	/// Inequality operator
	friend inline bool operator!=(const HashMapIterator &lhs, const HashMapIterator &rhs)
	{
		if (lhs.tag_ == SentinelTag::REGULAR && rhs.tag_ == SentinelTag::REGULAR)
			return (lhs.hashMap_ != rhs.hashMap_ || lhs.bucketIndex_ != rhs.bucketIndex_);
		else
			return (lhs.tag_ != rhs.tag_);
	}

	/// Returns the hashmap node currently pointed by the iterator
	typename HashMapHelperTraits<K, T, HashFunc, IsConst>::NodeReference node() const;
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

	typename HashMapHelperTraits<K, T, HashFunc, IsConst>::HashMapPtr hashMap_;
	unsigned int bucketIndex_;
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
	friend class HashMapIterator<K, T, HashFunc, true>;
};

/// Iterator traits structure specialization for `HashMapIterator` class
template <class K, class T, class HashFunc>
struct IteratorTraits<HashMapIterator<K, T, HashFunc, false>>
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

/// Iterator traits structure specialization for constant `HashMapIterator` class
template <class K, class T, class HashFunc>
struct IteratorTraits<HashMapIterator<K, T, HashFunc, true>>
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
HashMapIterator<K, T, HashFunc, IsConst>::HashMapIterator(typename HashMapHelperTraits<K, T, HashFunc, IsConst>::HashMapPtr hashMap, SentinelTagInit tag)
    : hashMap_(hashMap), bucketIndex_(0)
{
	switch (tag)
	{
		case SentinelTagInit::BEGINNING: tag_ = SentinelTag::BEGINNING; break;
		case SentinelTagInit::END: tag_ = SentinelTag::END; break;
	}
}

template <class K, class T, class HashFunc, bool IsConst>
typename HashMapIterator<K, T, HashFunc, IsConst>::Reference HashMapIterator<K, T, HashFunc, IsConst>::operator*() const
{
	return node().value;
}

template <class K, class T, class HashFunc, bool IsConst>
HashMapIterator<K, T, HashFunc, IsConst> &HashMapIterator<K, T, HashFunc, IsConst>::operator++()
{
	next();
	return *this;
}

template <class K, class T, class HashFunc, bool IsConst>
HashMapIterator<K, T, HashFunc, IsConst> HashMapIterator<K, T, HashFunc, IsConst>::operator++(int)
{
	// Create an unmodified copy to return
	HashMapIterator<K, T, HashFunc, IsConst> iterator = *this;
	next();
	return iterator;
}

template <class K, class T, class HashFunc, bool IsConst>
HashMapIterator<K, T, HashFunc, IsConst> &HashMapIterator<K, T, HashFunc, IsConst>::operator--()
{
	previous();
	return *this;
}

template <class K, class T, class HashFunc, bool IsConst>
HashMapIterator<K, T, HashFunc, IsConst> HashMapIterator<K, T, HashFunc, IsConst>::operator--(int)
{
	// Create an unmodified copy to return
	HashMapIterator<K, T, HashFunc, IsConst> iterator = *this;
	previous();
	return iterator;
}

template <class K, class T, class HashFunc, bool IsConst>
typename HashMapHelperTraits<K, T, HashFunc, IsConst>::NodeReference HashMapIterator<K, T, HashFunc, IsConst>::node() const
{
	return hashMap_->nodes_[bucketIndex_];
}

template <class K, class T, class HashFunc, bool IsConst>
const T &HashMapIterator<K, T, HashFunc, IsConst>::value() const
{
	return node().value;
}

template <class K, class T, class HashFunc, bool IsConst>
const K &HashMapIterator<K, T, HashFunc, IsConst>::key() const
{
	return node().key;
}

template <class K, class T, class HashFunc, bool IsConst>
hash_t HashMapIterator<K, T, HashFunc, IsConst>::hash() const
{
	return hashMap_->hashes_[bucketIndex_];
}

template <class K, class T, class HashFunc, bool IsConst>
void HashMapIterator<K, T, HashFunc, IsConst>::next()
{
	if (tag_ == SentinelTag::REGULAR)
	{
		if (bucketIndex_ >= hashMap_->capacity() - 1)
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
	while (bucketIndex_ < hashMap_->capacity() - 1 && hashMap_->hashes_[bucketIndex_] == NullHash)
		bucketIndex_++;

	if (hashMap_->hashes_[bucketIndex_] == NullHash)
		tag_ = SentinelTag::END;
}

template <class K, class T, class HashFunc, bool IsConst>
void HashMapIterator<K, T, HashFunc, IsConst>::previous()
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
		bucketIndex_ = hashMap_->capacity() - 1;
	}
	else if (tag_ == SentinelTag::BEGINNING)
		return;

	// Search the first non empty index starting from the current one
	while (bucketIndex_ > 0 && hashMap_->hashes_[bucketIndex_] == NullHash)
		bucketIndex_--;

	if (hashMap_->hashes_[bucketIndex_] == NullHash)
		tag_ = SentinelTag::BEGINNING;
}

}

#endif
