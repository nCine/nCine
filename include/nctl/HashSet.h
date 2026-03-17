#ifndef CLASS_NCTL_HASHSET
#define CLASS_NCTL_HASHSET

#include <new>
#include <initializer_list>
#include <ncine/common_macros.h>
#include "HashFunctions.h"
#include "ReverseIterator.h"
#include <cstring> // for memcpy()
#include "PointerMath.h"

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
template <class K, class HashFunc = FastHashFunc<K>>
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

	/// Constructs an hashset with explicit capacity
	explicit HashSet(unsigned int capacity);
	/// Constructs an hashset with an initializer list and explicit capacity
	HashSet(std::initializer_list<K> initList, unsigned int capacity);
#if NCINE_WITH_ALLOCATORS
	/// Constructs an hashset with explicit capacity and a custom allocator
	HashSet(unsigned int capacity, IAllocator &alloc);
	/// Constructs an hashset with an initializer list, an explicit capacity, and a custom allocator
	HashSet(std::initializer_list<K> initList, unsigned int capacity, IAllocator &alloc);
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

	/// Returns an iterator to the beginning
	Iterator begin();
	/// Returns a reverse iterator to the beginning
	inline ReverseIterator rBegin() { return ReverseIterator(end()); }
	/// Returns an iterator to the end
	ConstIterator end();
	/// Returns a reverse iterator to the end
	inline ReverseIterator rEnd() { return ReverseIterator(begin()); }

	/// Returns a constant iterator to the beginning
	ConstIterator begin() const;
	/// Returns a constant reverse iterator to the beginning
	inline ConstReverseIterator rBegin() const { return ConstReverseIterator(cEnd()); }
	/// Returns a constant iterator to the end
	ConstIterator end() const;
	/// Returns a constant reverse iterator to the end
	inline ConstReverseIterator rEnd() const { return ConstReverseIterator(cBegin()); }

	/// Returns a constant iterator to the beginning
	inline ConstIterator cBegin() const { return begin(); }
	/// Returns a constant reverse iterator to the beginning
	inline ConstReverseIterator crBegin() const { return rBegin(); }
	/// Returns a constant iterator to the end
	inline ConstIterator cEnd() const { return end(); }
	/// Returns a constant reverse iterator to the end
	inline ConstReverseIterator crEnd() const { return rEnd(); }

	/// Inserts an element if not already in and returns `true` on success
	inline bool insert(const K &key) { return insertImpl(key); }
	/// Moves an element if not already in and returns `true` on success
	inline bool insert(K &&key) { return insertImpl(nctl::move(key)); }
	/// Inserts elements from an initializer list
	bool insert(std::initializer_list<K> initList);

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
	K *find(const K &key);
	/// Checks whether an element is in the hashset or not (read-only)
	const K *find(const K &key) const;
	/// Checks whether an element is in the hashset or not
	bool contains(const K &key) const;
	/// Removes a key from the hashset, if it exists
	bool remove(const K &key);

	/// Sets the number of buckets to the new specified size and rehashes the container
	void rehash(unsigned int count);

  private:
	static const unsigned int AlignmentBytes = sizeof(int);

	/// The returning structure after probing for a key
	struct ProbeResult
	{
		/// The ideal bucket index for the hash
		unsigned int ideal;
		/// Only valid if the found flag is true
		unsigned int found;
		/// First empty node in a chain
		unsigned int empty;
		/// Previous node in a chain (for delta patching)
		unsigned int prev;
		/// True if the node contains a value
		bool foundFlag;
	};

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

	ProbeResult probe(const K &key, hash_t hash) const;
	unsigned int patchDeltas(const ProbeResult &r);
	unsigned int addDelta1(unsigned int bucketIndex) const;
	unsigned int addDelta2(unsigned int bucketIndex) const;
	unsigned int calcNewDelta(unsigned int bucketIndex, unsigned int newIndex) const;
	unsigned int linearSearch(unsigned int index, hash_t hash, const K &key) const;
	bool bucketFoundOrEmpty(unsigned int index, hash_t hash, const K &key) const;
	bool bucketFound(unsigned int index, hash_t hash, const K &key) const;
	template <class KeyArg> bool insertImpl(KeyArg &&key);

	void insertKey(unsigned int index, hash_t hash, const K &key);
	void insertKey(unsigned int index, hash_t hash, K &&key);

	friend class HashSetIterator<K, HashFunc>;
	friend struct HashSetHelperTraits<K, HashFunc>;
};

template <class K, class HashFunc>
inline typename HashSet<K, HashFunc>::Iterator HashSet<K, HashFunc>::begin()
{
	Iterator iterator(this, Iterator::SentinelTagInit::BEGINNING);
	return ++iterator;
}

template <class K, class HashFunc>
typename HashSet<K, HashFunc>::Iterator HashSet<K, HashFunc>::end()
{
	return Iterator(this, Iterator::SentinelTagInit::END);
}

template <class K, class HashFunc>
typename HashSet<K, HashFunc>::ConstIterator HashSet<K, HashFunc>::begin() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::BEGINNING);
	return ++iterator;
}

template <class K, class HashFunc>
typename HashSet<K, HashFunc>::ConstIterator HashSet<K, HashFunc>::end() const
{
	return ConstIterator(this, ConstIterator::SentinelTagInit::END);
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
	const unsigned int alignedBytes = bytes + 3 * AlignmentBytes; // 3 align adjustments in `initPointers()`
#if !NCINE_WITH_ALLOCATORS
	buffer_ = static_cast<uint8_t *>(::operator new(alignedBytes));
	keys_ = static_cast<K *>(::operator new(sizeof(K) * capacity_));
#else
	buffer_ = static_cast<uint8_t *>(alloc_.allocate(alignedBytes));
	keys_ = static_cast<K *>(alloc_.allocate(sizeof(K) * capacity_));
#endif

	initPointers();
	initValues();
}

template <class K, class HashFunc>
HashSet<K, HashFunc>::HashSet(std::initializer_list<K> initList, unsigned int capacity)
    : HashSet(capacity)
{
	insert(initList);
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

template <class K, class HashFunc>
HashSet<K, HashFunc>::HashSet(std::initializer_list<K> initList, unsigned int capacity, IAllocator &alloc)
    : HashSet(capacity, alloc)
{
	insert(initList);
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
	const unsigned int alignedBytes = bytes + 3 * AlignmentBytes; // 3 align adjustments in `initPointers()`
#if !NCINE_WITH_ALLOCATORS
	buffer_ = static_cast<uint8_t *>(::operator new(alignedBytes));
	keys_ = static_cast<K *>(::operator new(sizeof(K) * capacity_));
#else
	buffer_ = static_cast<uint8_t *>(alloc_.allocate(alignedBytes));
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
		const unsigned int alignedBytes = bytes + 3 * AlignmentBytes; // 3 align adjustments in `initPointers()`
#if !NCINE_WITH_ALLOCATORS
		buffer_ = static_cast<uint8_t *>(::operator new(alignedBytes));
		keys_ = static_cast<K *>(::operator new(sizeof(K) * capacity_));
#else
		buffer_ = static_cast<uint8_t *>(alloc_.allocate(alignedBytes));
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

/*! \return True if all initializer list elements have been inserted */
template <class K, class HashFunc>
bool HashSet<K, HashFunc>::insert(std::initializer_list<K> initList)
{
	const unsigned int MaxInsertions = capacity() - size();

	unsigned int numInserted = 0;
	for (const K &element : initList)
	{
		if (numInserted >= MaxInsertions)
			break;
		const bool inserted = insert(element);
		numInserted += inserted ? 1 : 0;
	}

	return (numInserted == initList.size());
}

template <class K, class HashFunc>
void HashSet<K, HashFunc>::clear()
{
	destructKeys();
	initValues();
}

/*! \note Prefer this method if copying `K` is expensive, but always check the validity of returned pointer. */
template <class K, class HashFunc>
K *HashSet<K, HashFunc>::find(const K &key)
{
	const hash_t hash = hashFunc_(key);
	const ProbeResult r = probe(key, hash);
	return (r.foundFlag) ? &keys_[r.found] : nullptr;
}

/*! \note Prefer this method if copying `K` is expensive, but always check the validity of returned pointer. */
template <class K, class HashFunc>
const K *HashSet<K, HashFunc>::find(const K &key) const
{
	const hash_t hash = hashFunc_(key);
	const ProbeResult r = probe(key, hash);
	return (r.foundFlag) ? &keys_[r.found] : nullptr;
}

template <class K, class HashFunc>
bool HashSet<K, HashFunc>::contains(const K &key) const
{
	return find(key) != nullptr;
}

/*! \return True if the element has been found and removed */
template <class K, class HashFunc>
bool HashSet<K, HashFunc>::remove(const K &key)
{
	if (size_ == 0)
		return false;

	const hash_t hash = hashFunc_(key);
	const ProbeResult r = probe(key, hash);

	if (!r.foundFlag)
		return false;

	unsigned int foundBucketIndex = r.found;
	unsigned int prevFoundBucketIndex = r.prev;
	unsigned int bucketIndex = foundBucketIndex;

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

	return true;
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
			hashSet.insert(nctl::move(keys_[i]));

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
	unsigned int alignAdjustment = PointerMath::alignAdjustment(buffer_, AlignmentBytes);
	uint8_t *pointer = reinterpret_cast<uint8_t*>(PointerMath::align(buffer_, AlignmentBytes));
	delta1_ = pointer;
	pointer += sizeof(uint8_t) * capacity_;

	alignAdjustment += PointerMath::alignAdjustment(pointer, AlignmentBytes);
	pointer = reinterpret_cast<uint8_t*>(PointerMath::align(pointer, AlignmentBytes));
	delta2_ = pointer;
	pointer += sizeof(uint8_t) * capacity_;

	alignAdjustment += PointerMath::alignAdjustment(pointer, AlignmentBytes);
	pointer = reinterpret_cast<uint8_t*>(PointerMath::align(pointer, AlignmentBytes));
	hashes_ = reinterpret_cast<hash_t *>(pointer);
	pointer += sizeof(hash_t) * capacity_;

	const unsigned int bytes = capacity_ * (sizeof(uint8_t) * 2 + sizeof(hash_t));
	FATAL_ASSERT(pointer == buffer_ + bytes + alignAdjustment);
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
typename HashSet<K, HashFunc>::ProbeResult
HashSet<K, HashFunc>::probe(const K &key, hash_t hash) const
{
	ProbeResult r{};
	r.ideal = hash % capacity_;
	r.prev = r.ideal;

	unsigned int bucketIndex = r.ideal;

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
					{
						r.found = bucketIndex;
						r.foundFlag = true;
						return r;
					}
				}

				// At this point, bucketIndex is the tail
				r.prev = bucketIndex; // for `patchDeltas()` to work

				// Adding at ideal index + delta1 + (n * delta2)
				r.empty = linearSearch(bucketIndex + 1, hash, key);
				r.foundFlag = false;
				return r;
			}
			else
			{
				// Found at ideal index + delta1
				r.found = bucketIndex;
				r.foundFlag = true;
				return r;
			}
		}
		else
		{
			// Adding at ideal index + delta1
			r.empty = linearSearch(bucketIndex + 1, hash, key);
			r.foundFlag = false;
			return r;
		}
	}
	else
	{
		// Using the ideal bucket index for the node
		if (hashes_[bucketIndex] == NullHash)
		{
			r.empty = bucketIndex;
			r.foundFlag = false;
			return r;
		}
		else
		{
			r.found = bucketIndex;
			r.foundFlag = true;
			return r;
		}
	}
}

template <class K, class HashFunc>
unsigned int HashSet<K, HashFunc>::patchDeltas(const ProbeResult &r)
{
	if (r.ideal == r.empty)
		return r.empty;

	if (delta1_[r.ideal] == 0)
		delta1_[r.ideal] = calcNewDelta(r.ideal, r.empty);
	else // r.prev must be the tail
		delta2_[r.prev] = calcNewDelta(r.prev, r.empty);

	return r.empty;
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
template <class KeyArg>
bool HashSet<K, HashFunc>::insertImpl(KeyArg &&key)
{
	const hash_t hash = hashFunc_(key);
	const ProbeResult r = probe(key, hash);

	if (r.foundFlag)
		return false;

	const unsigned int index = patchDeltas(r);
	insertKey(index, hash, nctl::forward<KeyArg>(key));
	return true;
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

}

#endif
