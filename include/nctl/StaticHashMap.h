#ifndef CLASS_NCTL_STATICHASHMAP
#define CLASS_NCTL_STATICHASHMAP

#include <ncine/common_macros.h>
#include "UniquePtr.h"
#include "HashFunctions.h"
#include "ReverseIterator.h"

namespace nctl {

template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst> class StaticHashMapIterator;
template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst> struct StaticHashMapHelperTraits;
class String;

/// A template based hashmap implementation with open addressing and leapfrog probing (version with static allocation)
template <class K, class T, unsigned int Capacity, class HashFunc = FNV1aHashFunc<K>>
class StaticHashMap
{
  public:
	/// Iterator type
	using Iterator = StaticHashMapIterator<K, T, HashFunc, Capacity, false>;
	/// Constant iterator type
	using ConstIterator = StaticHashMapIterator<K, T, HashFunc, Capacity, true>;
	/// Reverse iterator type
	using ReverseIterator = nctl::ReverseIterator<Iterator>;
	/// Reverse constant iterator type
	using ConstReverseIterator = nctl::ReverseIterator<ConstIterator>;

	StaticHashMap()
	    : size_(0) { clear(); }

	/// Copy constructor
	StaticHashMap(const StaticHashMap &other);
	/// Move constructor
	StaticHashMap(StaticHashMap &&other);
	/// Aassignment operator
	StaticHashMap &operator=(const StaticHashMap &other);
	/// Move aassignment operator
	StaticHashMap &operator=(StaticHashMap &&other);

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

	/// Subscript operator
	T &operator[](const K &key);
	/// Inserts an element if no other has the same key
	bool insert(const K &key, const T &value);
	/// Moves an element if no other has the same key
	bool insert(const K &key, T &&value);
	/// Constructs an element if no other has the same key
	template <typename... Args> bool emplace(const K &key, Args &&... args);

	/// Returns the capacity of the hashmap
	inline unsigned int capacity() const { return Capacity; }
	/// Returns true if the hashmap is empty
	inline bool isEmpty() const { return size_ == 0; }
	/// Returns the number of elements in the hashmap
	inline unsigned int size() const { return size_; }
	/// Returns the ratio between used and total buckets
	inline float loadFactor() const { return size_ / static_cast<float>(Capacity); }
	/// Returns the hash of a given key
	inline hash_t hash(const K &key) const { return hashFunc_(key); }

	/// Clears the hashmap
	void clear();
	/// Checks whether an element is in the hashmap or not
	bool contains(const K &key, T &returnedValue) const;
	/// Checks whether an element is in the hashmap or not
	T *find(const K &key);
	/// Checks whether an element is in the hashmap or not (read-only)
	const T *find(const K &key) const;
	/// Removes a key from the hashmap, if it exists
	bool remove(const K &key);

  private:
	/// The template class for the node stored inside the hashmap
	class Node
	{
	  public:
		K key;
		T value;
	};

	unsigned int size_;
	uint8_t delta1_[Capacity];
	uint8_t delta2_[Capacity];
	hash_t hashes_[Capacity];
	Node nodes_[Capacity];
	HashFunc hashFunc_;

	bool findBucketIndex(const K &key, unsigned int &foundIndex, unsigned int &prevFoundIndex) const;
	inline bool findBucketIndex(const K &key, unsigned int &foundIndex) const;
	unsigned int addDelta1(unsigned int bucketIndex) const;
	unsigned int addDelta2(unsigned int bucketIndex) const;
	unsigned int calcNewDelta(unsigned int bucketIndex, unsigned int newIndex) const;
	unsigned int linearSearch(unsigned int index, hash_t hash, const K &key) const;
	bool bucketFoundOrEmpty(unsigned int index, hash_t hash, const K &key) const;
	bool bucketFound(unsigned int index, hash_t hash, const K &key) const;
	T &addNode(unsigned int index, hash_t hash, const K &key);
	void insertNode(unsigned int index, hash_t hash, const K &key, const T &value);
	void insertNode(unsigned int index, hash_t hash, const K &key, T &&value);
	template <typename... Args> void emplaceNode(unsigned int index, hash_t hash, const K &key, Args &&... args);

	friend class StaticHashMapIterator<K, T, HashFunc, Capacity, false>;
	friend class StaticHashMapIterator<K, T, HashFunc, Capacity, true>;
	friend struct StaticHashMapHelperTraits<K, T, HashFunc, Capacity, false>;
	friend struct StaticHashMapHelperTraits<K, T, HashFunc, Capacity, true>;
};

template <class K, class T, unsigned int Capacity, class HashFunc>
typename StaticHashMap<K, T, Capacity, HashFunc>::Iterator StaticHashMap<K, T, Capacity, HashFunc>::begin()
{
	Iterator iterator(this, Iterator::SentinelTagInit::BEGINNING);
	return ++iterator;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
typename StaticHashMap<K, T, Capacity, HashFunc>::ReverseIterator StaticHashMap<K, T, Capacity, HashFunc>::rBegin()
{
	Iterator iterator(this, Iterator::SentinelTagInit::END);
	return ReverseIterator(--iterator);
}

template <class K, class T, unsigned int Capacity, class HashFunc>
typename StaticHashMap<K, T, Capacity, HashFunc>::Iterator StaticHashMap<K, T, Capacity, HashFunc>::end()
{
	return Iterator(this, Iterator::SentinelTagInit::END);
}

template <class K, class T, unsigned int Capacity, class HashFunc>
typename StaticHashMap<K, T, Capacity, HashFunc>::ReverseIterator StaticHashMap<K, T, Capacity, HashFunc>::rEnd()
{
	Iterator iterator(this, Iterator::SentinelTagInit::BEGINNING);
	return ReverseIterator(iterator);
}

template <class K, class T, unsigned int Capacity, class HashFunc>
inline typename StaticHashMap<K, T, Capacity, HashFunc>::ConstIterator StaticHashMap<K, T, Capacity, HashFunc>::begin() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::BEGINNING);
	return ++iterator;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
typename StaticHashMap<K, T, Capacity, HashFunc>::ConstReverseIterator StaticHashMap<K, T, Capacity, HashFunc>::rBegin() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::END);
	return ConstReverseIterator(--iterator);
}

template <class K, class T, unsigned int Capacity, class HashFunc>
inline typename StaticHashMap<K, T, Capacity, HashFunc>::ConstIterator StaticHashMap<K, T, Capacity, HashFunc>::end() const
{
	return ConstIterator(this, ConstIterator::SentinelTagInit::END);
}

template <class K, class T, unsigned int Capacity, class HashFunc>
typename StaticHashMap<K, T, Capacity, HashFunc>::ConstReverseIterator StaticHashMap<K, T, Capacity, HashFunc>::rEnd() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::BEGINNING);
	return ConstReverseIterator(iterator);
}

template <class K, class T, unsigned int Capacity, class HashFunc>
StaticHashMap<K, T, Capacity, HashFunc>::StaticHashMap(const StaticHashMap<K, T, Capacity, HashFunc> &other)
    : size_(other.size_)
{
	for (unsigned int i = 0; i < Capacity; i++)
	{
		delta1_[i] = other.delta1_[i];
		delta2_[i] = other.delta2_[i];
		hashes_[i] = other.hashes_[i];
		nodes_[i] = other.nodes_[i];
	}
}

template <class K, class T, unsigned int Capacity, class HashFunc>
StaticHashMap<K, T, Capacity, HashFunc>::StaticHashMap(StaticHashMap<K, T, Capacity, HashFunc> &&other)
    : size_(other.size_)
{
	for (unsigned int i = 0; i < Capacity; i++)
	{
		delta1_[i] = other.delta1_[i];
		delta2_[i] = other.delta2_[i];
		hashes_[i] = other.hashes_[i];
		nodes_[i] = nctl::move(other.nodes_[i]);
	}
}

template <class K, class T, unsigned int Capacity, class HashFunc>
StaticHashMap<K, T, Capacity, HashFunc> &StaticHashMap<K, T, Capacity, HashFunc>::operator=(const StaticHashMap<K, T, Capacity, HashFunc> &other)
{
	size_ = other.size_;

	for (unsigned int i = 0; i < Capacity; i++)
	{
		delta1_[i] = other.delta1_[i];
		delta2_[i] = other.delta2_[i];
		hashes_[i] = other.hashes_[i];
		nodes_[i] = other.nodes_[i];
	}

	return *this;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
StaticHashMap<K, T, Capacity, HashFunc> &StaticHashMap<K, T, Capacity, HashFunc>::operator=(StaticHashMap<K, T, Capacity, HashFunc> &&other)
{
	size_ = other.size_;

	for (unsigned int i = 0; i < Capacity; i++)
	{
		delta1_[i] = other.delta1_[i];
		delta2_[i] = other.delta2_[i];
		hashes_[i] = other.hashes_[i];
		nodes_[i] = nctl::move(other.nodes_[i]);
	}

	return *this;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
T &StaticHashMap<K, T, Capacity, HashFunc>::operator[](const K &key)
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
						return nodes_[bucketIndex].value;
				}

				// Adding at ideal index + delta1 + (n * delta2)
				const unsigned int newIndex = linearSearch(bucketIndex + 1, hash, key);
				delta2_[bucketIndex] = calcNewDelta(bucketIndex, newIndex);
				return addNode(newIndex, hash, key);
			}
			else
			{
				// Found at ideal index + delta1
				return nodes_[bucketIndex].value;
			}
		}
		else
		{
			// Adding at ideal index + delta1
			const unsigned int newIndex = linearSearch(bucketIndex + 1, hash, key);
			delta1_[bucketIndex] = calcNewDelta(bucketIndex, newIndex);
			return addNode(newIndex, hash, key);
		}
	}
	else
	{
		// Using the ideal bucket index for the node
		if (hashes_[bucketIndex] == NullHash)
			return addNode(bucketIndex, hash, key);
		else
			return nodes_[bucketIndex].value;
	}
}

/*! \return True if the element has been inserted */
template <class K, class T, unsigned int Capacity, class HashFunc>
bool StaticHashMap<K, T, Capacity, HashFunc>::insert(const K &key, const T &value)
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
				insertNode(newIndex, hash, key, value);
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
			insertNode(newIndex, hash, key, value);
			return true;
		}
	}
	else
	{
		// Using the ideal bucket index for the node
		if (hashes_[bucketIndex] == NullHash)
		{
			insertNode(bucketIndex, hash, key, value);
			return true;
		}
		else
			return false;
	}
}

/*! \return True if the element has been inserted */
template <class K, class T, unsigned int Capacity, class HashFunc>
bool StaticHashMap<K, T, Capacity, HashFunc>::insert(const K &key, T &&value)
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
				insertNode(newIndex, hash, key, nctl::move(value));
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
			insertNode(newIndex, hash, key, nctl::move(value));
			return true;
		}
	}
	else
	{
		// Using the ideal bucket index for the node
		if (hashes_[bucketIndex] == NullHash)
		{
			insertNode(bucketIndex, hash, key, nctl::move(value));
			return true;
		}
		else
			return false;
	}
}

/*! \return True if the element has been emplaced */
template <class K, class T, unsigned int Capacity, class HashFunc>
template <typename... Args>
bool StaticHashMap<K, T, Capacity, HashFunc>::emplace(const K &key, Args &&... args)
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
				emplaceNode(newIndex, hash, key, nctl::forward<Args>(args)...);
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
			emplaceNode(newIndex, hash, key, nctl::forward<Args>(args)...);
			return true;
		}
	}
	else
	{
		// Using the ideal bucket index for the node
		if (hashes_[bucketIndex] == NullHash)
		{
			emplaceNode(bucketIndex, hash, key, nctl::forward<Args>(args)...);
			return true;
		}
		else
			return false;
	}
}

template <class K, class T, unsigned int Capacity, class HashFunc>
void StaticHashMap<K, T, Capacity, HashFunc>::clear()
{
	for (unsigned int i = 0; i < Capacity; i++)
		delta1_[i] = 0;
	for (unsigned int i = 0; i < Capacity; i++)
		delta2_[i] = 0;
	for (unsigned int i = 0; i < Capacity; i++)
		hashes_[i] = NullHash;
	size_ = 0;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
bool StaticHashMap<K, T, Capacity, HashFunc>::contains(const K &key, T &returnedValue) const
{
	int unsigned bucketIndex = 0;
	const bool found = findBucketIndex(key, bucketIndex);

	if (found)
		returnedValue = nodes_[bucketIndex].value;

	return found;
}

/*! \note Prefer this method if copying `T` is expensive, but always check the validity of returned pointer. */
template <class K, class T, unsigned int Capacity, class HashFunc>
T *StaticHashMap<K, T, Capacity, HashFunc>::find(const K &key)
{
	int unsigned bucketIndex = 0;
	const bool found = findBucketIndex(key, bucketIndex);

	T *returnedPtr = nullptr;
	if (found)
		returnedPtr = &nodes_[bucketIndex].value;

	return returnedPtr;
}

/*! \note Prefer this method if copying `T` is expensive, but always check the validity of returned pointer. */
template <class K, class T, unsigned int Capacity, class HashFunc>
const T *StaticHashMap<K, T, Capacity, HashFunc>::find(const K &key) const
{
	int unsigned bucketIndex = 0;
	const bool found = findBucketIndex(key, bucketIndex);

	const T *returnedPtr = nullptr;
	if (found)
		returnedPtr = &nodes_[bucketIndex].value;

	return returnedPtr;
}

/*! \return True if the element has been found and removed */
template <class K, class T, unsigned int Capacity, class HashFunc>
bool StaticHashMap<K, T, Capacity, HashFunc>::remove(const K &key)
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
				nodes_[bucketIndex].key = nctl::move(nodes_[lastBucketIndex].key);
				nodes_[bucketIndex].value = nctl::move(nodes_[lastBucketIndex].value);
				hashes_[bucketIndex] = hashes_[lastBucketIndex];
			}

			bucketIndex = lastBucketIndex;
		}

		hashes_[bucketIndex] = NullHash;
		size_--;
	}

	return found;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
bool StaticHashMap<K, T, Capacity, HashFunc>::findBucketIndex(const K &key, unsigned int &foundIndex, unsigned int &prevFoundIndex) const
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

template <class K, class T, unsigned int Capacity, class HashFunc>
bool StaticHashMap<K, T, Capacity, HashFunc>::findBucketIndex(const K &key, unsigned int &foundIndex) const
{
	unsigned int prevFoundIndex = 0;
	return findBucketIndex(key, foundIndex, prevFoundIndex);
}

template <class K, class T, unsigned int Capacity, class HashFunc>
unsigned int StaticHashMap<K, T, Capacity, HashFunc>::addDelta1(unsigned int bucketIndex) const
{
	unsigned int newIndex = bucketIndex + delta1_[bucketIndex];
	if (newIndex > Capacity - 1)
		newIndex -= Capacity;
	return newIndex;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
unsigned int StaticHashMap<K, T, Capacity, HashFunc>::addDelta2(unsigned int bucketIndex) const
{
	unsigned int newIndex = bucketIndex + delta2_[bucketIndex];
	if (newIndex > Capacity - 1)
		newIndex -= Capacity;
	return newIndex;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
unsigned int StaticHashMap<K, T, Capacity, HashFunc>::calcNewDelta(unsigned int bucketIndex, unsigned int newIndex) const
{
	unsigned int delta = 0;
	if (newIndex >= bucketIndex)
		delta = newIndex - bucketIndex;
	else
		delta = Capacity - bucketIndex + newIndex;

	FATAL_ASSERT(delta < 256); // deltas are uint8_t
	return delta;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
unsigned int StaticHashMap<K, T, Capacity, HashFunc>::linearSearch(unsigned int index, hash_t hash, const K &key) const
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

template <class K, class T, unsigned int Capacity, class HashFunc>
bool StaticHashMap<K, T, Capacity, HashFunc>::bucketFoundOrEmpty(unsigned int index, hash_t hash, const K &key) const
{
	return (hashes_[index] == NullHash || (hashes_[index] == hash && nodes_[index].key == key));
}

template <class K, class T, unsigned int Capacity, class HashFunc>
bool StaticHashMap<K, T, Capacity, HashFunc>::bucketFound(unsigned int index, hash_t hash, const K &key) const
{
	return (hashes_[index] == hash && nodes_[index].key == key);
}

template <class K, class T, unsigned int Capacity, class HashFunc>
T &StaticHashMap<K, T, Capacity, HashFunc>::addNode(unsigned int index, hash_t hash, const K &key)
{
	FATAL_ASSERT(size_ < Capacity);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	nodes_[index].key = key;
	return nodes_[index].value;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
void StaticHashMap<K, T, Capacity, HashFunc>::insertNode(unsigned int index, hash_t hash, const K &key, const T &value)
{
	FATAL_ASSERT(size_ < Capacity);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	nodes_[index].key = key;
	nodes_[index].value = value;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
void StaticHashMap<K, T, Capacity, HashFunc>::insertNode(unsigned int index, hash_t hash, const K &key, T &&value)
{
	FATAL_ASSERT(size_ < Capacity);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	nodes_[index].key = key;
	nodes_[index].value = nctl::move(value);
}

template <class K, class T, unsigned int Capacity, class HashFunc>
template <typename... Args>
void StaticHashMap<K, T, Capacity, HashFunc>::emplaceNode(unsigned int index, hash_t hash, const K &key, Args &&... args)
{
	FATAL_ASSERT(size_ < Capacity);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	nodes_[index].key = key;
	new (&nodes_[index].value) T(nctl::forward<Args>(args)...);
}

template <class T, unsigned int Capacity>
using StaticStringHashMap = StaticHashMap<String, T, Capacity, FNV1aFuncHashContainer<String>>;

}

#endif
