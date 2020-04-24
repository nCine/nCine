#ifndef CLASS_NCTL_STATICHASHSET
#define CLASS_NCTL_STATICHASHSET

#include <ncine/common_macros.h>
#include "HashFunctions.h"
#include "ReverseIterator.h"

namespace nctl {

template <class K, class HashFunc, unsigned int Capacity> class StaticHashSetIterator;
template <class K, class HashFunc, unsigned int Capacity> struct StaticHashSetHelperTraits;
class String;

/// A template based hashset implementation with open addressing and leapfrog probing (version with static allocation)
template <class K, unsigned int Capacity, class HashFunc = FNV1aHashFunc<K>>
class StaticHashSet
{
  public:
	/// Iterator type
	using Iterator = StaticHashSetIterator<K, HashFunc, Capacity>;
	/// Constant iterator type
	using ConstIterator = StaticHashSetIterator<K, HashFunc, Capacity>;
	/// Reverse iterator type
	using ReverseIterator = nctl::ReverseIterator<Iterator>;
	/// Reverse constant iterator type
	using ConstReverseIterator = nctl::ReverseIterator<ConstIterator>;

	inline StaticHashSet()
	    : size_(0) { init(); }
	inline ~StaticHashSet() { destructKeys(); }

	/// Copy constructor
	StaticHashSet(const StaticHashSet &other);
	/// Move constructor
	StaticHashSet(StaticHashSet &&other);
	/// Aassignment operator
	StaticHashSet &operator=(const StaticHashSet &other);
	/// Move aassignment operator
	StaticHashSet &operator=(StaticHashSet &&other);

	/// Returns an iterator to the first element
	Iterator begin();
	/// Returns a reverse iterator to the last element
	ReverseIterator rBegin();
	/// Returns an iterator to past the last element
	Iterator end();
	/// Returns a reverse iterator to prior the first element
	ReverseIterator rEnd();

	/// Returns a constant iterator to the first element
	ConstIterator begin() const;
	/// Returns a constant reverse iterator to the last element
	ConstReverseIterator rBegin() const;
	/// Returns a constant iterator to past the last lement
	ConstIterator end() const;
	/// Returns a constant reverse iterator to prior the first element
	ConstReverseIterator rEnd() const;

	/// Returns a constant iterator to the first element
	inline ConstIterator cBegin() const { return begin(); }
	/// Returns a constant reverse iterator to the last element
	inline ConstReverseIterator crBegin() const { return rBegin(); }
	/// Returns a constant iterator to past the last lement
	inline ConstIterator cEnd() const { return end(); }
	/// Returns a constant reverse iterator to prior the first element
	inline ConstReverseIterator crEnd() const { return rEnd(); }

	/// Inserts an element if not already in
	bool insert(const K &key);
	/// Moves an element if not already in
	bool insert(K &&key);

	/// Returns the capacity of the hashset
	inline unsigned int capacity() const { return Capacity; }
	/// Returns true if the hashset is empty
	inline bool isEmpty() const { return size_ == 0; }
	/// Returns the number of elements in the hashset
	inline unsigned int size() const { return size_; }
	/// Returns the ratio between used and total buckets
	inline float loadFactor() const { return size_ / static_cast<float>(Capacity); }
	/// Returns the hash of a given key
	inline hash_t hash(const K &key) const { return hashFunc_(key); }

	/// Clears the hashset
	void clear();
	/// Checks whether an element is in the hashset or not
	bool contains(const K &key) const;
	/// Checks whether an element is in the hashset or not
	K *find(const K &key);
	/// Checks whether an element is in the hashset or not (read-only)
	const K *find(const K &key) const;
	/// Removes a key from the hashset, if it exists
	bool remove(const K &key);

  private:
	unsigned int size_;
	uint8_t delta1_[Capacity];
	uint8_t delta2_[Capacity];
	hash_t hashes_[Capacity];
	uint8_t keysBuffer_[Capacity * sizeof(K)];
	K *keys_ = reinterpret_cast<K *>(keysBuffer_);
	HashFunc hashFunc_;

	void init();
	void destructKeys();
	bool findBucketIndex(const K &key, unsigned int &foundIndex, unsigned int &prevFoundIndex) const;
	inline bool findBucketIndex(const K &key, unsigned int &foundIndex) const;
	unsigned int addDelta1(unsigned int bucketIndex) const;
	unsigned int addDelta2(unsigned int bucketIndex) const;
	unsigned int calcNewDelta(unsigned int bucketIndex, unsigned int newIndex) const;
	unsigned int linearSearch(unsigned int index, hash_t hash, const K &key) const;
	bool bucketFoundOrEmpty(unsigned int index, hash_t hash, const K &key) const;
	bool bucketFound(unsigned int index, hash_t hash, const K &key) const;
	void insertKey(unsigned int index, hash_t hash, const K &key);
	void insertKey(unsigned int index, hash_t hash, K &&key);

	friend class StaticHashSetIterator<K, HashFunc, Capacity>;
	friend struct StaticHashSetHelperTraits<K, HashFunc, Capacity>;
};

template <class K, unsigned int Capacity, class HashFunc>
typename StaticHashSet<K, Capacity, HashFunc>::Iterator StaticHashSet<K, Capacity, HashFunc>::begin()
{
	Iterator iterator(this, Iterator::SentinelTagInit::BEGINNING);
	return ++iterator;
}

template <class K, unsigned int Capacity, class HashFunc>
typename StaticHashSet<K, Capacity, HashFunc>::ReverseIterator StaticHashSet<K, Capacity, HashFunc>::rBegin()
{
	Iterator iterator(this, Iterator::SentinelTagInit::END);
	return ReverseIterator(--iterator);
}

template <class K, unsigned int Capacity, class HashFunc>
typename StaticHashSet<K, Capacity, HashFunc>::Iterator StaticHashSet<K, Capacity, HashFunc>::end()
{
	return Iterator(this, Iterator::SentinelTagInit::END);
}

template <class K, unsigned int Capacity, class HashFunc>
typename StaticHashSet<K, Capacity, HashFunc>::ReverseIterator StaticHashSet<K, Capacity, HashFunc>::rEnd()
{
	Iterator iterator(this, Iterator::SentinelTagInit::BEGINNING);
	return ReverseIterator(iterator);
}

template <class K, unsigned int Capacity, class HashFunc>
inline typename StaticHashSet<K, Capacity, HashFunc>::ConstIterator StaticHashSet<K, Capacity, HashFunc>::begin() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::BEGINNING);
	return ++iterator;
}

template <class K, unsigned int Capacity, class HashFunc>
typename StaticHashSet<K, Capacity, HashFunc>::ConstReverseIterator StaticHashSet<K, Capacity, HashFunc>::rBegin() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::END);
	return ConstReverseIterator(--iterator);
}

template <class K, unsigned int Capacity, class HashFunc>
inline typename StaticHashSet<K, Capacity, HashFunc>::ConstIterator StaticHashSet<K, Capacity, HashFunc>::end() const
{
	return ConstIterator(this, ConstIterator::SentinelTagInit::END);
}

template <class K, unsigned int Capacity, class HashFunc>
typename StaticHashSet<K, Capacity, HashFunc>::ConstReverseIterator StaticHashSet<K, Capacity, HashFunc>::rEnd() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::BEGINNING);
	return ConstReverseIterator(iterator);
}

template <class K, unsigned int Capacity, class HashFunc>
StaticHashSet<K, Capacity, HashFunc>::StaticHashSet(const StaticHashSet<K, Capacity, HashFunc> &other)
    : size_(other.size_)
{
	for (unsigned int i = 0; i < Capacity; i++)
	{
		if (other.hashes_[i] != NullHash)
			new (keys_ + i) K(other.keys_[i]);

		delta1_[i] = other.delta1_[i];
		delta2_[i] = other.delta2_[i];
		hashes_[i] = other.hashes_[i];
	}
}

template <class K, unsigned int Capacity, class HashFunc>
StaticHashSet<K, Capacity, HashFunc>::StaticHashSet(StaticHashSet<K, Capacity, HashFunc> &&other)
    : size_(other.size_)
{
	for (unsigned int i = 0; i < Capacity; i++)
	{
		if (other.hashes_[i] != NullHash)
			new (keys_ + i) K(nctl::move(other.keys_[i]));

		delta1_[i] = other.delta1_[i];
		delta2_[i] = other.delta2_[i];
		hashes_[i] = other.hashes_[i];
	}
	other.destructKeys();
}

template <class K, unsigned int Capacity, class HashFunc>
StaticHashSet<K, Capacity, HashFunc> &StaticHashSet<K, Capacity, HashFunc>::operator=(const StaticHashSet<K, Capacity, HashFunc> &other)
{
	for (unsigned int i = 0; i < Capacity; i++)
	{
		if (other.hashes_[i] != NullHash)
		{
			if (hashes_[i] != NullHash)
				keys_[i] = other.keys_[i];
			else
				new (keys_ + i) K(other.keys_[i]);
		}
		else if (hashes_[i] != NullHash)
			destructObject(keys_ + i);

		delta1_[i] = other.delta1_[i];
		delta2_[i] = other.delta2_[i];
		hashes_[i] = other.hashes_[i];
	}
	size_ = other.size_;

	return *this;
}

template <class K, unsigned int Capacity, class HashFunc>
StaticHashSet<K, Capacity, HashFunc> &StaticHashSet<K, Capacity, HashFunc>::operator=(StaticHashSet<K, Capacity, HashFunc> &&other)
{
	for (unsigned int i = 0; i < Capacity; i++)
	{
		if (other.hashes_[i] != NullHash)
		{
			if (hashes_[i] != NullHash)
				keys_[i] = nctl::move(other.keys_[i]);
			else
				new (keys_ + i) K(nctl::move(other.keys_[i]));
		}
		else if (hashes_[i] != NullHash)
			destructObject(keys_ + i);

		delta1_[i] = other.delta1_[i];
		delta2_[i] = other.delta2_[i];
		hashes_[i] = other.hashes_[i];
	}
	size_ = other.size_;
	other.destructKeys();

	return *this;
}

/*! \return True if the element has been inserted */
template <class K, unsigned int Capacity, class HashFunc>
bool StaticHashSet<K, Capacity, HashFunc>::insert(const K &key)
{
	const hash_t hash = hashFunc_(key);
	int unsigned bucketIndex = hash % Capacity;

	if (bucketFoundOrEmpty(bucketIndex, hash, key) == false)
	{
		if (delta1_[bucketIndex] != 0)
		{
			bucketIndex = addDelta1(bucketIndex);
			if (bucketFound(bucketIndex, hash, key) == false)
			{
				while (delta2_[bucketIndex] != 0)
				{
					bucketIndex = addDelta2(bucketIndex);
					// Found at ideal index + delta1 + (n * delta2)
					if (bucketFound(bucketIndex, hash, key))
						return false;
				}

				// Adding at ideal index + delta1 + (n * delta2)
				const unsigned int newIndex = linearSearch(bucketIndex + 1, hash, key);
				delta2_[bucketIndex] = calcNewDelta(bucketIndex, newIndex);
				insertKey(newIndex, hash, key);
				return true;
			}
			else
			{
				// Found at ideal index + delta1
				return false;
			}
		}
		else
		{
			// Adding at ideal index + delta1
			const unsigned int newIndex = linearSearch(bucketIndex + 1, hash, key);
			delta1_[bucketIndex] = calcNewDelta(bucketIndex, newIndex);
			insertKey(newIndex, hash, key);
			return true;
		}
	}
	else
	{
		// Using the ideal bucket index for the node
		if (hashes_[bucketIndex] == NullHash)
		{
			insertKey(bucketIndex, hash, key);
			return true;
		}
		else
			return false;
	}
}

/*! \return True if the element has been inserted */
template <class K, unsigned int Capacity, class HashFunc>
bool StaticHashSet<K, Capacity, HashFunc>::insert(K &&key)
{
	const hash_t hash = hashFunc_(key);
	int unsigned bucketIndex = hash % Capacity;

	if (bucketFoundOrEmpty(bucketIndex, hash, key) == false)
	{
		if (delta1_[bucketIndex] != 0)
		{
			bucketIndex = addDelta1(bucketIndex);
			if (bucketFound(bucketIndex, hash, key) == false)
			{
				while (delta2_[bucketIndex] != 0)
				{
					bucketIndex = addDelta2(bucketIndex);
					// Found at ideal index + delta1 + (n * delta2)
					if (bucketFound(bucketIndex, hash, key))
						return false;
				}

				// Adding at ideal index + delta1 + (n * delta2)
				const unsigned int newIndex = linearSearch(bucketIndex + 1, hash, key);
				delta2_[bucketIndex] = calcNewDelta(bucketIndex, newIndex);
				insertKey(newIndex, hash, nctl::move(key));
				return true;
			}
			else
			{
				// Found at ideal index + delta1
				return false;
			}
		}
		else
		{
			// Adding at ideal index + delta1
			const unsigned int newIndex = linearSearch(bucketIndex + 1, hash, key);
			delta1_[bucketIndex] = calcNewDelta(bucketIndex, newIndex);
			insertKey(newIndex, hash, nctl::move(key));
			return true;
		}
	}
	else
	{
		// Using the ideal bucket index for the node
		if (hashes_[bucketIndex] == NullHash)
		{
			insertKey(bucketIndex, hash, nctl::move(key));
			return true;
		}
		else
			return false;
	}
}

template <class K, unsigned int Capacity, class HashFunc>
void StaticHashSet<K, Capacity, HashFunc>::clear()
{
	destructKeys();
	init();
}

template <class K, unsigned int Capacity, class HashFunc>
bool StaticHashSet<K, Capacity, HashFunc>::contains(const K &key) const
{
	int unsigned bucketIndex = 0;
	return findBucketIndex(key, bucketIndex);
}

/*! \note Prefer this method if copying `T` is expensive, but always check the validity of returned pointer. */
template <class K, unsigned int Capacity, class HashFunc>
K *StaticHashSet<K, Capacity, HashFunc>::find(const K &key)
{
	int unsigned bucketIndex = 0;
	const bool found = findBucketIndex(key, bucketIndex);

	K *returnedPtr = nullptr;
	if (found)
		returnedPtr = &keys_[bucketIndex];

	return returnedPtr;
}

/*! \note Prefer this method if copying `T` is expensive, but always check the validity of returned pointer. */
template <class K, unsigned int Capacity, class HashFunc>
const K *StaticHashSet<K, Capacity, HashFunc>::find(const K &key) const
{
	int unsigned bucketIndex = 0;
	const bool found = findBucketIndex(key, bucketIndex);

	const K *returnedPtr = nullptr;
	if (found)
		returnedPtr = &keys_[bucketIndex];

	return returnedPtr;
}

/*! \return True if the element has been found and removed */
template <class K, unsigned int Capacity, class HashFunc>
bool StaticHashSet<K, Capacity, HashFunc>::remove(const K &key)
{
	int unsigned foundBucketIndex = 0;
	int unsigned prevFoundBucketIndex = 0;
	const bool found = findBucketIndex(key, foundBucketIndex, prevFoundBucketIndex);
	unsigned int bucketIndex = foundBucketIndex;

	if (found)
	{
		// The found bucket is the last of the chain, previous one needs a delta fix
		if (foundBucketIndex != hashes_[foundBucketIndex] % Capacity && delta2_[foundBucketIndex] == 0)
		{
			if (addDelta1(prevFoundBucketIndex) == foundBucketIndex)
				delta1_[prevFoundBucketIndex] = 0;
			else if (addDelta2(prevFoundBucketIndex) == foundBucketIndex)
				delta2_[prevFoundBucketIndex] = 0;
		}

		while (delta1_[bucketIndex] != 0 || delta2_[bucketIndex] != 0)
		{
			unsigned int lastBucketIndex = bucketIndex;
			if (delta1_[lastBucketIndex] != 0)
				lastBucketIndex = addDelta1(lastBucketIndex);
			if (delta2_[lastBucketIndex] != 0)
			{
				unsigned int secondLastBucketIndex = lastBucketIndex;
				while (delta2_[lastBucketIndex] != 0)
				{
					secondLastBucketIndex = lastBucketIndex;
					lastBucketIndex = addDelta2(lastBucketIndex);
				}
				delta2_[secondLastBucketIndex] = 0;
			}
			else
				delta1_[bucketIndex] = 0;

			if (bucketIndex != lastBucketIndex)
			{
				keys_[bucketIndex] = nctl::move(keys_[lastBucketIndex]);
				hashes_[bucketIndex] = hashes_[lastBucketIndex];
			}

			bucketIndex = lastBucketIndex;
		}

		hashes_[bucketIndex] = NullHash;
		destructObject(keys_ + bucketIndex);
		size_--;
	}

	return found;
}

template <class K, unsigned int Capacity, class HashFunc>
void StaticHashSet<K, Capacity, HashFunc>::init()
{
	for (unsigned int i = 0; i < Capacity; i++)
		delta1_[i] = 0;
	for (unsigned int i = 0; i < Capacity; i++)
		delta2_[i] = 0;
	for (unsigned int i = 0; i < Capacity; i++)
		hashes_[i] = NullHash;
}

template <class K, unsigned int Capacity, class HashFunc>
void StaticHashSet<K, Capacity, HashFunc>::destructKeys()
{
	for (unsigned int i = 0; i < Capacity; i++)
	{
		if (hashes_[i] != NullHash)
		{
			destructObject(keys_ + i);
			hashes_[i] = NullHash;
		}
	}
	size_ = 0;
}

template <class K, unsigned int Capacity, class HashFunc>
bool StaticHashSet<K, Capacity, HashFunc>::findBucketIndex(const K &key, unsigned int &foundIndex, unsigned int &prevFoundIndex) const
{
	if (size_ == 0)
		return false;

	bool found = false;
	const hash_t hash = hashFunc_(key);
	foundIndex = hash % Capacity;
	prevFoundIndex = foundIndex;

	if (bucketFoundOrEmpty(foundIndex, hash, key) == false)
	{
		if (delta1_[foundIndex] != 0)
		{
			prevFoundIndex = foundIndex;
			foundIndex = addDelta1(foundIndex);
			if (bucketFound(foundIndex, hash, key) == false)
			{
				while (delta2_[foundIndex] != 0)
				{
					prevFoundIndex = foundIndex;
					foundIndex = addDelta2(foundIndex);
					if (bucketFound(foundIndex, hash, key))
					{
						// Found at ideal index + delta1 + (n * delta2)
						found = true;
						break;
					}
				}
			}
			else
			{
				// Found at ideal index + delta1
				found = true;
			}
		}
	}
	else
	{
		if (hashes_[foundIndex] != NullHash)
		{
			// Found at ideal bucket index
			found = true;
		}
	}

	return found;
}

template <class K, unsigned int Capacity, class HashFunc>
bool StaticHashSet<K, Capacity, HashFunc>::findBucketIndex(const K &key, unsigned int &foundIndex) const
{
	unsigned int prevFoundIndex = 0;
	return findBucketIndex(key, foundIndex, prevFoundIndex);
}

template <class K, unsigned int Capacity, class HashFunc>
unsigned int StaticHashSet<K, Capacity, HashFunc>::addDelta1(unsigned int bucketIndex) const
{
	unsigned int newIndex = bucketIndex + delta1_[bucketIndex];
	if (newIndex > Capacity - 1)
		newIndex -= Capacity;
	return newIndex;
}

template <class K, unsigned int Capacity, class HashFunc>
unsigned int StaticHashSet<K, Capacity, HashFunc>::addDelta2(unsigned int bucketIndex) const
{
	unsigned int newIndex = bucketIndex + delta2_[bucketIndex];
	if (newIndex > Capacity - 1)
		newIndex -= Capacity;
	return newIndex;
}

template <class K, unsigned int Capacity, class HashFunc>
unsigned int StaticHashSet<K, Capacity, HashFunc>::calcNewDelta(unsigned int bucketIndex, unsigned int newIndex) const
{
	unsigned int delta = 0;
	if (newIndex >= bucketIndex)
		delta = newIndex - bucketIndex;
	else
		delta = Capacity - bucketIndex + newIndex;

	FATAL_ASSERT(delta < 256); // deltas are uint8_t
	return delta;
}

template <class K, unsigned int Capacity, class HashFunc>
unsigned int StaticHashSet<K, Capacity, HashFunc>::linearSearch(unsigned int index, hash_t hash, const K &key) const
{
	for (unsigned int i = index; i < Capacity; i++)
	{
		if (bucketFoundOrEmpty(i, hash, key))
			return i;
	}

	for (unsigned int i = 0; i < index; i++)
	{
		if (bucketFoundOrEmpty(i, hash, key))
			return i;
	}

	return index;
}

template <class K, unsigned int Capacity, class HashFunc>
bool StaticHashSet<K, Capacity, HashFunc>::bucketFoundOrEmpty(unsigned int index, hash_t hash, const K &key) const
{
	return (hashes_[index] == NullHash || (hashes_[index] == hash && equalTo(keys_[index], key)));
}

template <class K, unsigned int Capacity, class HashFunc>
bool StaticHashSet<K, Capacity, HashFunc>::bucketFound(unsigned int index, hash_t hash, const K &key) const
{
	return (hashes_[index] == hash && equalTo(keys_[index], key));
}

template <class K, unsigned int Capacity, class HashFunc>
void StaticHashSet<K, Capacity, HashFunc>::insertKey(unsigned int index, hash_t hash, const K &key)
{
	FATAL_ASSERT(size_ < Capacity);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	new (keys_ + index) K(key);
}

template <class K, unsigned int Capacity, class HashFunc>
void StaticHashSet<K, Capacity, HashFunc>::insertKey(unsigned int index, hash_t hash, K &&key)
{
	FATAL_ASSERT(size_ < Capacity);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	new (keys_ + index) K(nctl::move(key));
}

template <unsigned int Capacity>
using StaticStringHashSet = StaticHashSet<String, Capacity, FNV1aHashFuncContainer<String>>;

template <unsigned int Capacity>
using StaticCStringHashSet = StaticHashSet<const char *, Capacity, FNV1aHashFunc<const char *>>;

}

#endif
