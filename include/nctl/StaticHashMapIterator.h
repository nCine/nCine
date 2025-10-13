#ifndef CLASS_NCTL_STATICHASHMAPITERATOR
#define CLASS_NCTL_STATICHASHMAPITERATOR

#include "StaticHashMap.h"
#include "iterator.h"

namespace nctl {

/// Base helper structure for type traits used in the hashmap iterator
template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst>
struct StaticHashMapHelperTraits
{};

/// Helper structure providing type traits used in the non constant hashmap iterator
template <class K, class T, class HashFunc, unsigned int Capacity>
struct StaticHashMapHelperTraits<K, T, HashFunc, Capacity, false>
{
	using HashMapPtr = StaticHashMap<K, T, Capacity, HashFunc> *;
	using NodeReference = typename StaticHashMap<K, T, Capacity, HashFunc>::Node &;
};

/// Helper structure providing type traits used in the constant hashmap iterator
template <class K, class T, class HashFunc, unsigned int Capacity>
struct StaticHashMapHelperTraits<K, T, HashFunc, Capacity, true>
{
	using HashMapPtr = const StaticHashMap<K, T, Capacity, HashFunc> *;
	using NodeReference = const typename StaticHashMap<K, T, Capacity, HashFunc>::Node &;
};

/// A hashmap iterator
template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst>
class StaticHashMapIterator
{
  public:
	/// Reference type which respects iterator constness
	using Reference = typename IteratorTraits<StaticHashMapIterator>::Reference;

	/// Sentinel tags to initialize the iterator at the beginning and end
	enum class SentinelTagInit
	{
		/// Iterator at the beginning, next element is the first one
		BEGINNING,
		/// Iterator at the end, previous element is the last one
		END
	};

	StaticHashMapIterator(typename StaticHashMapHelperTraits<K, T, HashFunc, Capacity, IsConst>::HashMapPtr hashMap, unsigned int bucketIndex)
	    : hashMap_(hashMap), bucketIndex_(bucketIndex), tag_(SentinelTag::REGULAR) {}

	StaticHashMapIterator(typename StaticHashMapHelperTraits<K, T, HashFunc, Capacity, IsConst>::HashMapPtr hashMap, SentinelTagInit tag);

	/// Copy constructor to implicitly convert a non constant iterator to a constant one
	StaticHashMapIterator(const StaticHashMapIterator<K, T, HashFunc, Capacity, false> &it)
	    : hashMap_(it.hashMap_), bucketIndex_(it.bucketIndex_), tag_(SentinelTag(it.tag_)) {}

	/// Deferencing operator
	Reference operator*() const;

	/// Iterates to the next element (prefix)
	StaticHashMapIterator &operator++();
	/// Iterates to the next element (postfix)
	StaticHashMapIterator operator++(int);

	/// Iterates to the previous element (prefix)
	StaticHashMapIterator &operator--();
	/// Iterates to the previous element (postfix)
	StaticHashMapIterator operator--(int);

	/// Equality operator
	friend inline bool operator==(const StaticHashMapIterator &lhs, const StaticHashMapIterator &rhs)
	{
		if (lhs.tag_ == SentinelTag::REGULAR && rhs.tag_ == SentinelTag::REGULAR)
			return (lhs.hashMap_ == rhs.hashMap_ && lhs.bucketIndex_ == rhs.bucketIndex_);
		else
			return (lhs.tag_ == rhs.tag_);
	}

	/// Inequality operator
	friend inline bool operator!=(const StaticHashMapIterator &lhs, const StaticHashMapIterator &rhs)
	{
		if (lhs.tag_ == SentinelTag::REGULAR && rhs.tag_ == SentinelTag::REGULAR)
			return (lhs.hashMap_ != rhs.hashMap_ || lhs.bucketIndex_ != rhs.bucketIndex_);
		else
			return (lhs.tag_ != rhs.tag_);
	}

	/// Returns the hashmap node currently pointed by the iterator
	typename StaticHashMapHelperTraits<K, T, HashFunc, Capacity, IsConst>::NodeReference node() const;
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

	typename StaticHashMapHelperTraits<K, T, HashFunc, Capacity, IsConst>::HashMapPtr hashMap_;
	unsigned int bucketIndex_;
	SentinelTag tag_;

	/// Makes the iterator point to the next element in the hashmap
	void next();
	/// Makes the iterator point to the previous element in the hashmap
	void previous();

	/// For non constant to constant iterator implicit conversion
	friend class StaticHashMapIterator<K, T, HashFunc, Capacity, true>;
};

/// Iterator traits structure specialization for `HashMapIterator` class
template <class K, class T, class HashFunc, unsigned int Capacity>
struct IteratorTraits<StaticHashMapIterator<K, T, HashFunc, Capacity, false>>
{
	/// Type of the values deferenced by the iterator
	using ValueType = T;
	/// Pointer to the type of the values deferenced by the iterator
	using Pointer = T *;
	/// Reference to the type of the values deferenced by the iterator
	using Reference = T &;
	/// Type trait for iterator category
	using IteratorCategory = BidirectionalIteratorTag;
};

/// Iterator traits structure specialization for constant `HashMapIterator` class
template <class K, class T, class HashFunc, unsigned int Capacity>
struct IteratorTraits<StaticHashMapIterator<K, T, HashFunc, Capacity, true>>
{
	/// Type of the values deferenced by the iterator (never const)
	using ValueType = T;
	/// Pointer to the type of the values deferenced by the iterator
	using Pointer = const T *;
	/// Reference to the type of the values deferenced by the iterator
	using Reference = const T &;
	/// Type trait for iterator category
	using IteratorCategory = BidirectionalIteratorTag;
};

template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst>
StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst>::StaticHashMapIterator(typename StaticHashMapHelperTraits<K, T, HashFunc, Capacity, IsConst>::HashMapPtr hashMap, SentinelTagInit tag)
    : hashMap_(hashMap), bucketIndex_(0)
{
	switch (tag)
	{
		case SentinelTagInit::BEGINNING: tag_ = SentinelTag::BEGINNING; break;
		case SentinelTagInit::END: tag_ = SentinelTag::END; break;
	}
}

template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst>
typename StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst>::Reference StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst>::operator*() const
{
	return node().value;
}

template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst>
StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst> &StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst>::operator++()
{
	next();
	return *this;
}

template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst>
StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst> StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst>::operator++(int)
{
	// Create an unmodified copy to return
	StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst> iterator = *this;
	next();
	return iterator;
}

template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst>
StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst> &StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst>::operator--()
{
	previous();
	return *this;
}

template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst>
StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst> StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst>::operator--(int)
{
	// Create an unmodified copy to return
	StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst> iterator = *this;
	previous();
	return iterator;
}

template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst>
typename StaticHashMapHelperTraits<K, T, HashFunc, Capacity, IsConst>::NodeReference StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst>::node() const
{
	return hashMap_->nodes_[bucketIndex_];
}

template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst>
const T &StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst>::value() const
{
	return node().value;
}

template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst>
const K &StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst>::key() const
{
	return node().key;
}

template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst>
hash_t StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst>::hash() const
{
	return hashMap_->hashes_[bucketIndex_];
}

template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst>
void StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst>::next()
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

template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst>
void StaticHashMapIterator<K, T, HashFunc, Capacity, IsConst>::previous()
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
