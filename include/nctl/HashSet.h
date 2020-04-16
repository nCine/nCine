#ifndef CLASS_NCTL_HASHSET
#define CLASS_NCTL_HASHSET

#include <ncine/common_macros.h>
#include "HashFunctions.h"
#include "ReverseIterator.h"
#include <cstring> // for memcpy()

#include <ncine/config.h>
#if NCINE_WITH_ALLOCATORS
	#include "AllocManager.h"
	#include "IAllocator.h"
#endif

namespace nctl {

template <class K, class HashFunc> class HashSetIterator;
template <class K, class HashFunc> struct HashSetHelperTraits;
class String;

/// A template based hashset implementation with open addressing and leapfrog probing
template <class K, class HashFunc = FNV1aHashFunc<K>>
class HashSet
{
  public:
	/// Iterator type
	/*! Elements in the hashset can never be changed */
	using Iterator = HashSetIterator<K, HashFunc>;
	/// Constant iterator type
	using ConstIterator = HashSetIterator<K, HashFunc>;
	/// Reverse iterator type
	using ReverseIterator = nctl::ReverseIterator<Iterator>;
	/// Reverse constant iterator type
	using ConstReverseIterator = nctl::ReverseIterator<ConstIterator>;

	explicit HashSet(unsigned int capacity);
#if NCINE_WITH_ALLOCATORS
	HashSet(unsigned int capacity, IAllocator &alloc);
#endif
	~HashSet();

	/// Copy constructor
	HashSet(const HashSet &other);
	/// Move constructor
	HashSet(HashSet &&other);
	/// Assignment operator
	HashSet &operator=(const HashSet &other);
	/// Move assignment operator
	HashSet &operator=(HashSet &&other);

	/// Swaps two hashsets without copying their data
	inline void swap(HashSet &first, HashSet &second)
	{
#if NCINE_WITH_ALLOCATORS
		nctl::swap(first.alloc_, second.alloc_);
#endif
		nctl::swap(first.size_, second.size_);
		nctl::swap(first.capacity_, second.capacity_);
		nctl::swap(first.buffer_, second.buffer_);
		nctl::swap(first.delta1_, second.delta1_);
		nctl::swap(first.delta2_, second.delta2_);
		nctl::swap(first.hashes_, second.hashes_);
		nctl::swap(first.keys_, second.keys_);
	}

	/// Returns a constant iterator to the first element
	ConstIterator begin();
	/// Returns a reverse constant iterator to the last element
	ConstReverseIterator rBegin();
	/// Returns a constant iterator to past the last element
	ConstIterator end();
	/// Returns a reverse constant iterator to prior the first element
	ConstReverseIterator rEnd();

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
	inline unsigned int capacity() const { return capacity_; }
	/// Returns true if the hashset is empty
	inline bool isEmpty() const { return size_ == 0; }
	/// Returns the number of elements in the hashset
	inline unsigned int size() const { return size_; }
	/// Returns the ratio between used and total buckets
	inline float loadFactor() const { return size_ / static_cast<float>(capacity_); }
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

	/// Sets the number of buckets to the new specified size and rehashes the container
	void rehash(unsigned int count);

  private:
#if NCINE_WITH_ALLOCATORS
	/// The custom memory allocator for the hashset
	IAllocator &alloc_;
#endif
	unsigned int size_;
	unsigned int capacity_;
	/// Single allocated buffer for all the hashset per-node data
	uint8_t *buffer_;
	uint8_t *delta1_;
	uint8_t *delta2_;
	hash_t *hashes_;
	K *keys_;
	HashFunc hashFunc_;

	void initPointers();
	void initValues();
	void destructKeys();
	void deallocate();
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

	friend class HashSetIterator<K, HashFunc>;
	friend struct HashSetHelperTraits<K, HashFunc>;
};

template <class K, class HashFunc>
inline typename HashSet<K, HashFunc>::ConstIterator HashSet<K, HashFunc>::begin()
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::BEGINNING);
	return ++iterator;
}

template <class K, class HashFunc>
typename HashSet<K, HashFunc>::ConstReverseIterator HashSet<K, HashFunc>::rBegin()
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::END);
	return ConstReverseIterator(--iterator);
}

template <class K, class HashFunc>
typename HashSet<K, HashFunc>::ConstIterator HashSet<K, HashFunc>::end()
{
	return ConstIterator(this, ConstIterator::SentinelTagInit::END);
}

template <class K, class HashFunc>
typename HashSet<K, HashFunc>::ConstReverseIterator HashSet<K, HashFunc>::rEnd()
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::BEGINNING);
	return ConstReverseIterator(iterator);
}

template <class K, class HashFunc>
typename HashSet<K, HashFunc>::ConstIterator HashSet<K, HashFunc>::begin() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::BEGINNING);
	return ++iterator;
}

template <class K, class HashFunc>
typename HashSet<K, HashFunc>::ConstReverseIterator HashSet<K, HashFunc>::rBegin() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::END);
	return ConstReverseIterator(--iterator);
}

template <class K, class HashFunc>
typename HashSet<K, HashFunc>::ConstIterator HashSet<K, HashFunc>::end() const
{
	return ConstIterator(this, ConstIterator::SentinelTagInit::END);
}

template <class K, class HashFunc>
typename HashSet<K, HashFunc>::ConstReverseIterator HashSet<K, HashFunc>::rEnd() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::BEGINNING);
	return ConstReverseIterator(iterator);
}

template <class K, class HashFunc>
HashSet<K, HashFunc>::HashSet(unsigned int capacity)
    :
#if NCINE_WITH_ALLOCATORS
      alloc_(theDefaultAllocator()),
#endif
      size_(0), capacity_(capacity), buffer_(nullptr),
      delta1_(nullptr), delta2_(nullptr), hashes_(nullptr), keys_(nullptr)
{
	FATAL_ASSERT_MSG(capacity > 0, "Zero is not a valid capacity");

	const unsigned int bytes = capacity_ * (sizeof(uint8_t) * 2 + sizeof(hash_t));
#if !NCINE_WITH_ALLOCATORS
	buffer_ = static_cast<uint8_t *>(::operator new(bytes));
	keys_ = static_cast<K *>(::operator new(sizeof(K) * capacity_));
#else
	buffer_ = static_cast<uint8_t *>(alloc_.allocate(bytes));
	keys_ = static_cast<K *>(alloc_.allocate(sizeof(K) * capacity_));
#endif

	initPointers();
	initValues();
}

#if NCINE_WITH_ALLOCATORS
template <class K, class HashFunc>
HashSet<K, HashFunc>::HashSet(unsigned int capacity, IAllocator &alloc)
    : alloc_(alloc), size_(0), capacity_(capacity), buffer_(nullptr),
      delta1_(nullptr), delta2_(nullptr), hashes_(nullptr), keys_(nullptr)
{
	FATAL_ASSERT_MSG(capacity > 0, "Zero is not a valid capacity");

	const unsigned int bytes = capacity_ * (sizeof(uint8_t) * 2 + sizeof(hash_t));
	buffer_ = static_cast<uint8_t *>(alloc_.allocate(bytes));
	keys_ = static_cast<K *>(alloc_.allocate(sizeof(K) * capacity_));

	initPointers();
	initValues();
}
#endif

template <class K, class HashFunc>
HashSet<K, HashFunc>::~HashSet()
{
	destructKeys();
	deallocate();
}

template <class K, class HashFunc>
HashSet<K, HashFunc>::HashSet(const HashSet<K, HashFunc> &other)
    :
#if NCINE_WITH_ALLOCATORS
      alloc_(other.alloc_),
#endif
      size_(other.size_), capacity_(other.capacity_), buffer_(nullptr),
      delta1_(nullptr), delta2_(nullptr), hashes_(nullptr), keys_(nullptr)
{
	const unsigned int bytes = capacity_ * (sizeof(uint8_t) * 2 + sizeof(hash_t));
#if !NCINE_WITH_ALLOCATORS
	buffer_ = static_cast<uint8_t *>(::operator new(bytes));
	keys_ = static_cast<K *>(::operator new(sizeof(K) * capacity_));
#else
	buffer_ = static_cast<uint8_t *>(alloc_.allocate(bytes));
	keys_ = static_cast<K *>(alloc_.allocate(sizeof(K) * capacity_));
#endif
	memcpy(buffer_, other.buffer_, bytes);
	initPointers();

	for (unsigned int i = 0; i < capacity_; i++)
	{
		if (other.hashes_[i] != NullHash)
			new (keys_ + i) K(other.keys_[i]);
	}
}

template <class K, class HashFunc>
HashSet<K, HashFunc>::HashSet(HashSet<K, HashFunc> &&other)
    :
#if NCINE_WITH_ALLOCATORS
      alloc_(other.alloc_),
#endif
      size_(other.size_), capacity_(other.capacity_), buffer_(other.buffer_),
      delta1_(other.delta1_), delta2_(other.delta2_), hashes_(other.hashes_), keys_(other.keys_)
{
	other.size_ = 0;
	other.capacity_ = 0;
	other.buffer_ = nullptr;
	other.delta1_ = nullptr;
	other.delta2_ = nullptr;
	other.hashes_ = nullptr;
	other.keys_ = nullptr;
}

template <class K, class HashFunc>
HashSet<K, HashFunc> &HashSet<K, HashFunc>::operator=(const HashSet<K, HashFunc> &other)
{
	if (this == &other)
		return *this;

	if (other.size_ > capacity_)
	{
		destructKeys();
		deallocate();

		capacity_ = other.capacity_;
		const unsigned int bytes = capacity_ * (sizeof(uint8_t) * 2 + sizeof(hash_t));
#if !NCINE_WITH_ALLOCATORS
		buffer_ = static_cast<uint8_t *>(::operator new(bytes));
		keys_ = static_cast<K *>(::operator new(sizeof(K) * capacity_));
#else
		buffer_ = static_cast<uint8_t *>(alloc_.allocate(bytes));
		keys_ = static_cast<K *>(alloc_.allocate(sizeof(K) * capacity_));
#endif
		initPointers();
	}

	for (unsigned int i = 0; i < capacity_; i++)
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

template <class K, class HashFunc>
HashSet<K, HashFunc> &HashSet<K, HashFunc>::operator=(HashSet<K, HashFunc> &&other)
{
	if (this != &other)
	{
		swap(*this, other);
		other.clear();
	}
	return *this;
}

/*! \return True if the element has been inserted */
template <class K, class HashFunc>
bool HashSet<K, HashFunc>::insert(const K &key)
{
	const hash_t hash = hashFunc_(key);
	int unsigned bucketIndex = hash % capacity_;

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
template <class K, class HashFunc>
bool HashSet<K, HashFunc>::insert(K &&key)
{
	const hash_t hash = hashFunc_(key);
	int unsigned bucketIndex = hash % capacity_;

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

template <class K, class HashFunc>
void HashSet<K, HashFunc>::clear()
{
	destructKeys();
	initValues();
}

template <class K, class HashFunc>
bool HashSet<K, HashFunc>::contains(const K &key) const
{
	int unsigned bucketIndex = 0;
	return findBucketIndex(key, bucketIndex);
}

/*! \note Prefer this method if copying `K` is expensive, but always check the validity of returned pointer. */
template <class K, class HashFunc>
K *HashSet<K, HashFunc>::find(const K &key)
{
	int unsigned bucketIndex = 0;
	const bool found = findBucketIndex(key, bucketIndex);

	K *returnedPtr = nullptr;
	if (found)
		returnedPtr = &keys_[bucketIndex];

	return returnedPtr;
}

/*! \note Prefer this method if copying `K` is expensive, but always check the validity of returned pointer. */
template <class K, class HashFunc>
const K *HashSet<K, HashFunc>::find(const K &key) const
{
	int unsigned bucketIndex = 0;
	const bool found = findBucketIndex(key, bucketIndex);

	const K *returnedPtr = nullptr;
	if (found)
		returnedPtr = &keys_[bucketIndex];

	return returnedPtr;
}

/*! \return True if the element has been found and removed */
template <class K, class HashFunc>
bool HashSet<K, HashFunc>::remove(const K &key)
{
	int unsigned foundBucketIndex = 0;
	int unsigned prevFoundBucketIndex = 0;
	const bool found = findBucketIndex(key, foundBucketIndex, prevFoundBucketIndex);
	unsigned int bucketIndex = foundBucketIndex;

	if (found)
	{
		// The found bucket is the last of the chain, previous one needs a delta fix
		if (foundBucketIndex != hashes_[foundBucketIndex] % capacity_ && delta2_[foundBucketIndex] == 0)
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

template <class K, class HashFunc>
void HashSet<K, HashFunc>::rehash(unsigned int count)
{
	if (size_ == 0 || count < size_)
		return;

	HashSet<K, HashFunc> hashSet(count);

	unsigned int rehashedKeys = 0;
	for (unsigned int i = 0; i < capacity_; i++)
	{
		if (hashes_[i] != NullHash)
		{
			hashSet.insert(keys_[i]);

			rehashedKeys++;
			if (rehashedKeys == size_)
				break;
		}
	}

	*this = nctl::move(hashSet);
}

template <class K, class HashFunc>
void HashSet<K, HashFunc>::initPointers()
{
	uint8_t *pointer = buffer_;
	delta1_ = pointer;
	pointer += sizeof(uint8_t) * capacity_;
	delta2_ = pointer;
	pointer += sizeof(uint8_t) * capacity_;
	hashes_ = reinterpret_cast<hash_t *>(pointer);
	pointer += sizeof(hash_t) * capacity_;

	const unsigned int bytes = capacity_ * (sizeof(uint8_t) * 2 + sizeof(hash_t));
	FATAL_ASSERT(pointer == buffer_ + bytes);
}

template <class K, class HashFunc>
void HashSet<K, HashFunc>::initValues()
{
	for (unsigned int i = 0; i < capacity_; i++)
		delta1_[i] = 0;
	for (unsigned int i = 0; i < capacity_; i++)
		delta2_[i] = 0;
	for (unsigned int i = 0; i < capacity_; i++)
		hashes_[i] = NullHash;
}

template <class K, class HashFunc>
void HashSet<K, HashFunc>::destructKeys()
{
	for (unsigned int i = 0; i < capacity_; i++)
	{
		if (hashes_[i] != NullHash)
		{
			destructObject(keys_ + i);
			hashes_[i] = NullHash;
		}
	}
	size_ = 0;
}

template <class K, class HashFunc>
void HashSet<K, HashFunc>::deallocate()
{
#if !NCINE_WITH_ALLOCATORS
	::operator delete(buffer_);
	::operator delete(keys_);
#else
	alloc_.deallocate(buffer_);
	alloc_.deallocate(keys_);
#endif
}

template <class K, class HashFunc>
bool HashSet<K, HashFunc>::findBucketIndex(const K &key, unsigned int &foundIndex, unsigned int &prevFoundIndex) const
{
	if (size_ == 0)
		return false;

	bool found = false;
	const hash_t hash = hashFunc_(key);
	foundIndex = hash % capacity_;
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

template <class K, class HashFunc>
bool HashSet<K, HashFunc>::findBucketIndex(const K &key, unsigned int &foundIndex) const
{
	unsigned int prevFoundIndex = 0;
	return findBucketIndex(key, foundIndex, prevFoundIndex);
}

template <class K, class HashFunc>
unsigned int HashSet<K, HashFunc>::addDelta1(unsigned int bucketIndex) const
{
	unsigned int newIndex = bucketIndex + delta1_[bucketIndex];
	if (newIndex > capacity_ - 1)
		newIndex -= capacity_;
	return newIndex;
}

template <class K, class HashFunc>
unsigned int HashSet<K, HashFunc>::addDelta2(unsigned int bucketIndex) const
{
	unsigned int newIndex = bucketIndex + delta2_[bucketIndex];
	if (newIndex > capacity_ - 1)
		newIndex -= capacity_;
	return newIndex;
}

template <class K, class HashFunc>
unsigned int HashSet<K, HashFunc>::calcNewDelta(unsigned int bucketIndex, unsigned int newIndex) const
{
	unsigned int delta = 0;
	if (newIndex >= bucketIndex)
		delta = newIndex - bucketIndex;
	else
		delta = capacity_ - bucketIndex + newIndex;

	FATAL_ASSERT(delta < 256); // deltas are uint8_t
	return delta;
}

template <class K, class HashFunc>
unsigned int HashSet<K, HashFunc>::linearSearch(unsigned int index, hash_t hash, const K &key) const
{
	for (unsigned int i = index; i < capacity_; i++)
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

template <class K, class HashFunc>
bool HashSet<K, HashFunc>::bucketFoundOrEmpty(unsigned int index, hash_t hash, const K &key) const
{
	return (hashes_[index] == NullHash || (hashes_[index] == hash && equalTo(keys_[index], key)));
}

template <class K, class HashFunc>
bool HashSet<K, HashFunc>::bucketFound(unsigned int index, hash_t hash, const K &key) const
{
	return (hashes_[index] == hash && equalTo(keys_[index], key));
}

template <class K, class HashFunc>
void HashSet<K, HashFunc>::insertKey(unsigned int index, hash_t hash, const K &key)
{
	FATAL_ASSERT(size_ < capacity_);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	new (keys_ + index) K(key);
}

template <class K, class HashFunc>
void HashSet<K, HashFunc>::insertKey(unsigned int index, hash_t hash, K &&key)
{
	FATAL_ASSERT(size_ < capacity_);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	new (keys_ + index) K(nctl::move(key));
}

using StringHashSet = HashSet<String, FNV1aHashFuncContainer<String>>;
using CStringHashSet = HashSet<const char *, FNV1aHashFunc<const char *>>;

}

#endif
