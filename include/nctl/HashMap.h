#ifndef CLASS_NCTL_HASHMAP
#define CLASS_NCTL_HASHMAP

#include <new>
#include <initializer_list>
#include <ncine/common_macros.h>
#include "HashFunctions.h"
#include "Pair.h"
#include "ReverseIterator.h"
#include <cstring> // for memcpy()
#include "PointerMath.h"

#include <ncine/config.h>
#if NCINE_WITH_ALLOCATORS
	#include "AllocManager.h"
	#include "IAllocator.h"
#endif

namespace nctl {

template <class K, class T, class HashFunc, bool IsConst> class HashMapIterator;
template <class K, class T, class HashFunc, bool IsConst> struct HashMapHelperTraits;
class String;

/// A template based hashmap implementation with open addressing and leapfrog probing
template <class K, class T, class HashFunc = FNV1aHashFunc<K>>
class HashMap
{
  public:
	/// Iterator type
	using Iterator = HashMapIterator<K, T, HashFunc, false>;
	/// Constant iterator type
	using ConstIterator = HashMapIterator<K, T, HashFunc, true>;
	/// Reverse iterator type
	using ReverseIterator = nctl::ReverseIterator<Iterator>;
	/// Reverse constant iterator type
	using ConstReverseIterator = nctl::ReverseIterator<ConstIterator>;

	/// Constructs an hashmap with explicit capacity
	explicit HashMap(unsigned int capacity);
	/// Constructs an hashmap with an initializer list and explicit capacity
	HashMap(std::initializer_list<Pair<K, T>> initList, unsigned int capacity);
#if NCINE_WITH_ALLOCATORS
	/// Constructs an hashmap with explicit capacity and a custom allocator
	HashMap(unsigned int capacity, IAllocator &alloc);
	/// Constructs an hashmap with an initializer list, an explicit capacity, and a custom allocator
	HashMap(std::initializer_list<Pair<K, T>> initList, unsigned int capacity, IAllocator &alloc);
#endif
	~HashMap();

	/// Copy constructor
	HashMap(const HashMap &other);
	/// Move constructor
	HashMap(HashMap &&other);
	/// Assignment operator
	HashMap &operator=(const HashMap &other);
	/// Move assignment operator
	HashMap &operator=(HashMap &&other);

	/// Swaps two hashmaps without copying their data
	inline void swap(HashMap &first, HashMap &second)
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
		nctl::swap(first.nodes_, second.nodes_);
	}

	/// Returns an iterator to the beginning
	Iterator begin();
	/// Returns a reverse iterator to the beginning
	inline ReverseIterator rBegin() { return ReverseIterator(end()); }
	/// Returns an iterator to the end
	Iterator end();
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

	/// Subscript operator
	T &operator[](const K &key);
	/// Inserts an element if no other has the same key and returns `true` on success
	inline bool insert(const K &key, const T &value) { return insertImpl(key, value); }
	/// Moves an element if no other has the same key and returns `true` on success
	inline bool insert(const K &key, T &&value) { return insertImpl(key, nctl::move(value)); }
	/// Inserts an element from a pair, if no other has the same key, and returns `true` on success
	inline bool insert(const Pair<K, T> &pair) { return insertImpl(pair.first, pair.second); }
	/// Inserts elements from an initializer list of pairs
	bool insert(std::initializer_list<Pair<K, T>> initList);
	/// Constructs an element if no other has the same key
	template <typename... Args> bool emplace(const K &key, Args &&... args);

	/// Returns the capacity of the hashmap
	inline unsigned int capacity() const { return capacity_; }
	/// Returns true if the hashmap is empty
	inline bool isEmpty() const { return size_ == 0; }
	/// Returns the number of elements in the hashmap
	inline unsigned int size() const { return size_; }
	/// Returns the ratio between used and total buckets
	inline float loadFactor() const { return size_ / static_cast<float>(capacity_); }
	/// Returns the hash of a given key
	inline hash_t hash(const K &key) const { return hashFunc_(key); }

	/// Clears the hashmap
	void clear();
	/// Checks whether an element is in the hashmap or not
	T *find(const K &key);
	/// Checks whether an element is in the hashmap or not (read-only)
	const T *find(const K &key) const;
	/// Checks whether an element is in the hashmap or not
	bool contains(const K &key) const;
	/// Removes a key from the hashmap, if it exists
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

	/// The template class for the node stored inside the hashmap
	class Node
	{
	  public:
		K key;
		T value;

		Node() {}
		explicit Node(K kk)
		    : key(kk) {}
		Node(K kk, const T &vv)
		    : key(kk), value(vv) {}
		Node(K kk, T &&vv)
		    : key(kk), value(nctl::move(vv)) {}
		template <typename... Args>
		Node(K kk, Args &&... args)
		    : key(kk), value(nctl::forward<Args>(args)...) {}
	};

#if NCINE_WITH_ALLOCATORS
	/// The custom memory allocator for the hashmap
	IAllocator &alloc_;
#endif
	unsigned int size_;
	unsigned int capacity_;
	/// Single allocated buffer for all the hashmap per-node data
	uint8_t *buffer_;
	uint8_t *delta1_;
	uint8_t *delta2_;
	hash_t *hashes_;
	Node *nodes_;
	HashFunc hashFunc_;

	void initPointers();
	void initValues();
	void destructNodes();
	void deallocate();

	ProbeResult probe(const K &key, hash_t hash) const;
	unsigned int patchDeltas(const ProbeResult &r);
	unsigned int addDelta1(unsigned int bucketIndex) const;
	unsigned int addDelta2(unsigned int bucketIndex) const;
	unsigned int calcNewDelta(unsigned int bucketIndex, unsigned int newIndex) const;
	unsigned int linearSearch(unsigned int index, hash_t hash, const K &key) const;
	bool bucketFoundOrEmpty(unsigned int index, hash_t hash, const K &key) const;
	bool bucketFound(unsigned int index, hash_t hash, const K &key) const;
	template <class ValueArg> bool insertImpl(const K &key, ValueArg &&value);

	T &addNode(unsigned int index, hash_t hash, const K &key);
	void insertNode(unsigned int index, hash_t hash, const K &key, const T &value);
	void insertNode(unsigned int index, hash_t hash, const K &key, T &&value);
	template <typename... Args> void emplaceNode(unsigned int index, hash_t hash, const K &key, Args &&... args);

	friend class HashMapIterator<K, T, HashFunc, false>;
	friend class HashMapIterator<K, T, HashFunc, true>;
	friend struct HashMapHelperTraits<K, T, HashFunc, false>;
	friend struct HashMapHelperTraits<K, T, HashFunc, true>;
};

template <class K, class T, class HashFunc>
inline typename HashMap<K, T, HashFunc>::Iterator HashMap<K, T, HashFunc>::begin()
{
	Iterator iterator(this, Iterator::SentinelTagInit::BEGINNING);
	return ++iterator;
}

template <class K, class T, class HashFunc>
typename HashMap<K, T, HashFunc>::Iterator HashMap<K, T, HashFunc>::end()
{
	return Iterator(this, Iterator::SentinelTagInit::END);
}

template <class K, class T, class HashFunc>
typename HashMap<K, T, HashFunc>::ConstIterator HashMap<K, T, HashFunc>::begin() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::BEGINNING);
	return ++iterator;
}

template <class K, class T, class HashFunc>
typename HashMap<K, T, HashFunc>::ConstIterator HashMap<K, T, HashFunc>::end() const
{
	return ConstIterator(this, ConstIterator::SentinelTagInit::END);
}

template <class K, class T, class HashFunc>
HashMap<K, T, HashFunc>::HashMap(unsigned int capacity)
    :
#if NCINE_WITH_ALLOCATORS
      alloc_(theDefaultAllocator()),
#endif
      size_(0), capacity_(capacity), buffer_(nullptr),
      delta1_(nullptr), delta2_(nullptr), hashes_(nullptr), nodes_(nullptr)
{
	FATAL_ASSERT_MSG(capacity > 0, "Zero is not a valid capacity");

	const unsigned int bytes = capacity_ * (sizeof(uint8_t) * 2 + sizeof(hash_t));
	const unsigned int alignedBytes = bytes + 3 * AlignmentBytes; // 3 align adjustments in `initPointers()`
#if !NCINE_WITH_ALLOCATORS
	buffer_ = static_cast<uint8_t *>(::operator new(alignedBytes));
	nodes_ = static_cast<Node *>(::operator new(sizeof(Node) * capacity_));
#else
	buffer_ = static_cast<uint8_t *>(alloc_.allocate(alignedBytes));
	nodes_ = static_cast<Node *>(alloc_.allocate(sizeof(Node) * capacity_));
#endif

	uint8_t *pointer = buffer_;
	delta1_ = pointer;
	pointer += sizeof(uint8_t) * capacity_;
	delta2_ = pointer;
	pointer += sizeof(uint8_t) * capacity_;
	hashes_ = reinterpret_cast<hash_t *>(pointer);
	pointer += sizeof(hash_t) * capacity_;
	FATAL_ASSERT(pointer == buffer_ + bytes);

	initValues();
}

template <class K, class T, class HashFunc>
HashMap<K, T, HashFunc>::HashMap(std::initializer_list<Pair<K, T>> initList, unsigned int capacity)
    : HashMap(capacity)
{
	insert(initList);
}

#if NCINE_WITH_ALLOCATORS
template <class K, class T, class HashFunc>
HashMap<K, T, HashFunc>::HashMap(unsigned int capacity, IAllocator &alloc)
    : alloc_(alloc), size_(0), capacity_(capacity), buffer_(nullptr),
      delta1_(nullptr), delta2_(nullptr), hashes_(nullptr), nodes_(nullptr)
{
	FATAL_ASSERT_MSG(capacity > 0, "Zero is not a valid capacity");

	const unsigned int bytes = capacity_ * (sizeof(uint8_t) * 2 + sizeof(hash_t));
	buffer_ = static_cast<uint8_t *>(alloc_.allocate(bytes));
	nodes_ = static_cast<Node *>(alloc_.allocate(sizeof(Node) * capacity_));

	initPointers();
	initValues();
}

template <class K, class T, class HashFunc>
HashMap<K, T, HashFunc>::HashMap(std::initializer_list<Pair<K, T>> initList, unsigned int capacity, IAllocator &alloc)
    : HashMap(capacity, alloc)
{
	insert(initList);
}
#endif

template <class K, class T, class HashFunc>
HashMap<K, T, HashFunc>::~HashMap()
{
	destructNodes();
	deallocate();
}

template <class K, class T, class HashFunc>
HashMap<K, T, HashFunc>::HashMap(const HashMap<K, T, HashFunc> &other)
    :
#if NCINE_WITH_ALLOCATORS
      alloc_(other.alloc_),
#endif
      size_(other.size_), capacity_(other.capacity_), buffer_(nullptr),
      delta1_(nullptr), delta2_(nullptr), hashes_(nullptr), nodes_(nullptr)
{
	const unsigned int bytes = capacity_ * (sizeof(uint8_t) * 2 + sizeof(hash_t));
	const unsigned int alignedBytes = bytes + 3 * AlignmentBytes; // 3 align adjustments in `initPointers()`
#if !NCINE_WITH_ALLOCATORS
	buffer_ = static_cast<uint8_t *>(::operator new(alignedBytes));
	nodes_ = static_cast<Node *>(::operator new(sizeof(Node) * capacity_));
#else
	buffer_ = static_cast<uint8_t *>(alloc_.allocate(alignedBytes));
	nodes_ = static_cast<Node *>(alloc_.allocate(sizeof(Node) * capacity_));
#endif
	memcpy(buffer_, other.buffer_, bytes);
	initPointers();

	for (unsigned int i = 0; i < capacity_; i++)
	{
		if (other.hashes_[i] != NullHash)
			new (nodes_ + i) Node(other.nodes_[i]);
	}
}

template <class K, class T, class HashFunc>
HashMap<K, T, HashFunc>::HashMap(HashMap<K, T, HashFunc> &&other)
    :
#if NCINE_WITH_ALLOCATORS
      alloc_(other.alloc_),
#endif
      size_(other.size_), capacity_(other.capacity_), buffer_(other.buffer_),
      delta1_(other.delta1_), delta2_(other.delta2_), hashes_(other.hashes_), nodes_(other.nodes_)
{
	other.size_ = 0;
	other.capacity_ = 0;
	other.buffer_ = nullptr;
	other.delta1_ = nullptr;
	other.delta2_ = nullptr;
	other.hashes_ = nullptr;
	other.nodes_ = nullptr;
}

template <class K, class T, class HashFunc>
HashMap<K, T, HashFunc> &HashMap<K, T, HashFunc>::operator=(const HashMap<K, T, HashFunc> &other)
{
	if (this == &other)
		return *this;

	if (other.size_ > capacity_)
	{
		destructNodes();
		deallocate();

		capacity_ = other.capacity_;
		const unsigned int bytes = capacity_ * (sizeof(uint8_t) * 2 + sizeof(hash_t));
		const unsigned int alignedBytes = bytes + 3 * AlignmentBytes; // 3 align adjustments in `initPointers()`
#if !NCINE_WITH_ALLOCATORS
		buffer_ = static_cast<uint8_t *>(::operator new(alignedBytes));
		nodes_ = static_cast<Node *>(::operator new(sizeof(Node) * capacity_));
#else
		buffer_ = static_cast<uint8_t *>(alloc_.allocate(alignedBytes));
		nodes_ = static_cast<Node *>(alloc_.allocate(sizeof(Node) * capacity_));
#endif
		initPointers();
	}

	for (unsigned int i = 0; i < capacity_; i++)
	{
		if (other.hashes_[i] != NullHash)
		{
			if (hashes_[i] != NullHash)
				nodes_[i] = other.nodes_[i];
			else
				new (nodes_ + i) Node(other.nodes_[i]);
		}
		else if (hashes_[i] != NullHash)
			destructObject(nodes_ + i);

		delta1_[i] = other.delta1_[i];
		delta2_[i] = other.delta2_[i];
		hashes_[i] = other.hashes_[i];
	}
	size_ = other.size_;

	return *this;
}

template <class K, class T, class HashFunc>
HashMap<K, T, HashFunc> &HashMap<K, T, HashFunc>::operator=(HashMap<K, T, HashFunc> &&other)
{
	if (this != &other)
	{
		swap(*this, other);
		other.clear();
	}
	return *this;
}

template <class K, class T, class HashFunc>
T &HashMap<K, T, HashFunc>::operator[](const K &key)
{
	const hash_t hash = hashFunc_(key);
	const ProbeResult r = probe(key, hash);

	if (r.foundFlag)
		return nodes_[r.found].value;

	const unsigned int index = patchDeltas(r);
	return addNode(index, hash, key);
}

/*! \return True if all initializer list elements have been inserted */
template <class K, class T, class HashFunc>
bool HashMap<K, T, HashFunc>::insert(std::initializer_list<Pair<K, T>> initList)
{
	const unsigned int MaxInsertions = capacity() - size();

	unsigned int numInserted = 0;
	for (const Pair<K, T> &element : initList)
	{
		if (numInserted >= MaxInsertions)
			break;
		const bool inserted = insert(element);
		numInserted += inserted ? 1 : 0;
	}

	return (numInserted == initList.size());
}

/*! \return True if the element has been emplaced */
template <class K, class T, class HashFunc>
template <typename... Args>
bool HashMap<K, T, HashFunc>::emplace(const K &key, Args &&... args)
{
	const hash_t hash = hashFunc_(key);
	const ProbeResult r = probe(key, hash);

	if (r.foundFlag)
		return false;

	const unsigned int index = patchDeltas(r);
	emplaceNode(index, hash, key, nctl::forward<Args>(args)...);
	return true;
}

template <class K, class T, class HashFunc>
void HashMap<K, T, HashFunc>::clear()
{
	destructNodes();
	initValues();
}

/*! \note Prefer this method if copying `T` is expensive, but always check the validity of returned pointer. */
template <class K, class T, class HashFunc>
T *HashMap<K, T, HashFunc>::find(const K &key)
{
	const hash_t hash = hashFunc_(key);
	const ProbeResult r = probe(key, hash);
	return (r.foundFlag) ? &nodes_[r.found].value : nullptr;
}

/*! \note Prefer this method if copying `T` is expensive, but always check the validity of returned pointer. */
template <class K, class T, class HashFunc>
const T *HashMap<K, T, HashFunc>::find(const K &key) const
{
	const hash_t hash = hashFunc_(key);
	const ProbeResult r = probe(key, hash);
	return (r.foundFlag) ? &nodes_[r.found].value : nullptr;
}

template <class K, class T, class HashFunc>
bool HashMap<K, T, HashFunc>::contains(const K &key) const
{
	return find(key) != nullptr;
}

/*! \return True if the element has been found and removed */
template <class K, class T, class HashFunc>
bool HashMap<K, T, HashFunc>::remove(const K &key)
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
			nodes_[bucketIndex].key = nctl::move(nodes_[lastBucketIndex].key);
			nodes_[bucketIndex].value = nctl::move(nodes_[lastBucketIndex].value);
			hashes_[bucketIndex] = hashes_[lastBucketIndex];
		}

		bucketIndex = lastBucketIndex;
	}

	hashes_[bucketIndex] = NullHash;
	destructObject(nodes_ + bucketIndex);
	size_--;

	return true;
}

template <class K, class T, class HashFunc>
void HashMap<K, T, HashFunc>::rehash(unsigned int count)
{
	if (size_ == 0 || count < size_)
		return;

	HashMap<K, T, HashFunc> hashMap(count);

	unsigned int rehashedNodes = 0;
	for (unsigned int i = 0; i < capacity_; i++)
	{
		if (hashes_[i] != NullHash)
		{
			Node &node = nodes_[i];
			hashMap.insert(node.key, nctl::move(node.value));

			rehashedNodes++;
			if (rehashedNodes == size_)
				break;
		}
	}

	*this = nctl::move(hashMap);
}

template <class K, class T, class HashFunc>
void HashMap<K, T, HashFunc>::initPointers()
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

template <class K, class T, class HashFunc>
void HashMap<K, T, HashFunc>::initValues()
{
	for (unsigned int i = 0; i < capacity_; i++)
		delta1_[i] = 0;
	for (unsigned int i = 0; i < capacity_; i++)
		delta2_[i] = 0;
	for (unsigned int i = 0; i < capacity_; i++)
		hashes_[i] = NullHash;
}

template <class K, class T, class HashFunc>
void HashMap<K, T, HashFunc>::destructNodes()
{
	for (unsigned int i = 0; i < capacity_; i++)
	{
		if (hashes_[i] != NullHash)
		{
			destructObject(nodes_ + i);
			hashes_[i] = NullHash;
		}
	}
	size_ = 0;
}

template <class K, class T, class HashFunc>
void HashMap<K, T, HashFunc>::deallocate()
{
#if !NCINE_WITH_ALLOCATORS
	::operator delete(buffer_);
	::operator delete(nodes_);
#else
	alloc_.deallocate(buffer_);
	alloc_.deallocate(nodes_);
#endif
}

template <class K, class T, class HashFunc>
typename HashMap<K, T, HashFunc>::ProbeResult
HashMap<K, T, HashFunc>::probe(const K &key, hash_t hash) const
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

template <class K, class T, class HashFunc>
unsigned int HashMap<K, T, HashFunc>::patchDeltas(const ProbeResult &r)
{
	if (r.ideal == r.empty)
		return r.empty;

	if (delta1_[r.ideal] == 0)
		delta1_[r.ideal] = calcNewDelta(r.ideal, r.empty);
	else // r.prev must be the tail
		delta2_[r.prev] = calcNewDelta(r.prev, r.empty);

	return r.empty;
}

template <class K, class T, class HashFunc>
unsigned int HashMap<K, T, HashFunc>::addDelta1(unsigned int bucketIndex) const
{
	unsigned int newIndex = bucketIndex + delta1_[bucketIndex];
	if (newIndex > capacity_ - 1)
		newIndex -= capacity_;
	return newIndex;
}

template <class K, class T, class HashFunc>
unsigned int HashMap<K, T, HashFunc>::addDelta2(unsigned int bucketIndex) const
{
	unsigned int newIndex = bucketIndex + delta2_[bucketIndex];
	if (newIndex > capacity_ - 1)
		newIndex -= capacity_;
	return newIndex;
}

template <class K, class T, class HashFunc>
unsigned int HashMap<K, T, HashFunc>::calcNewDelta(unsigned int bucketIndex, unsigned int newIndex) const
{
	unsigned int delta = 0;
	if (newIndex >= bucketIndex)
		delta = newIndex - bucketIndex;
	else
		delta = capacity_ - bucketIndex + newIndex;

	FATAL_ASSERT(delta < 256); // deltas are uint8_t
	return delta;
}

template <class K, class T, class HashFunc>
unsigned int HashMap<K, T, HashFunc>::linearSearch(unsigned int index, hash_t hash, const K &key) const
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

template <class K, class T, class HashFunc>
bool HashMap<K, T, HashFunc>::bucketFoundOrEmpty(unsigned int index, hash_t hash, const K &key) const
{
	return (hashes_[index] == NullHash || (hashes_[index] == hash && equalTo(nodes_[index].key, key)));
}

template <class K, class T, class HashFunc>
bool HashMap<K, T, HashFunc>::bucketFound(unsigned int index, hash_t hash, const K &key) const
{
	return (hashes_[index] == hash && equalTo(nodes_[index].key, key));
}

template <class K, class T, class HashFunc>
template <class ValueArg>
bool HashMap<K, T, HashFunc>::insertImpl(const K &key, ValueArg &&value)
{
	const hash_t hash = hashFunc_(key);
	const ProbeResult r = probe(key, hash);

	if (r.foundFlag)
		return false;

	const unsigned int index = patchDeltas(r);
	insertNode(index, hash, key, nctl::forward<ValueArg>(value));
	return true;
}

template <class K, class T, class HashFunc>
T &HashMap<K, T, HashFunc>::addNode(unsigned int index, hash_t hash, const K &key)
{
	FATAL_ASSERT(size_ < capacity_);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	new (nodes_ + index) Node(key);

	return nodes_[index].value;
}

template <class K, class T, class HashFunc>
void HashMap<K, T, HashFunc>::insertNode(unsigned int index, hash_t hash, const K &key, const T &value)
{
	FATAL_ASSERT(size_ < capacity_);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	new (nodes_ + index) Node(key, value);
}

template <class K, class T, class HashFunc>
void HashMap<K, T, HashFunc>::insertNode(unsigned int index, hash_t hash, const K &key, T &&value)
{
	FATAL_ASSERT(size_ < capacity_);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	new (nodes_ + index) Node(key, nctl::move(value));
}

template <class K, class T, class HashFunc>
template <typename... Args>
void HashMap<K, T, HashFunc>::emplaceNode(unsigned int index, hash_t hash, const K &key, Args &&... args)
{
	FATAL_ASSERT(size_ < capacity_);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	new (nodes_ + index) Node(key, nctl::forward<Args>(args)...);
}

}

#endif
