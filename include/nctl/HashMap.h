#ifndef CLASS_NCTL_HASHMAP
#define CLASS_NCTL_HASHMAP

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

	explicit HashMap(unsigned int capacity);
#if NCINE_WITH_ALLOCATORS
	HashMap(unsigned int capacity, IAllocator &alloc);
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
	bool contains(const K &key, T &returnedValue) const;
	/// Checks whether an element is in the hashmap or not
	T *find(const K &key);
	/// Checks whether an element is in the hashmap or not (read-only)
	const T *find(const K &key) const;
	/// Removes a key from the hashmap, if it exists
	bool remove(const K &key);

	/// Sets the number of buckets to the new specified size and rehashes the container
	void rehash(unsigned int count);

  private:
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
typename HashMap<K, T, HashFunc>::ReverseIterator HashMap<K, T, HashFunc>::rBegin()
{
	Iterator iterator(this, Iterator::SentinelTagInit::END);
	return ReverseIterator(--iterator);
}

template <class K, class T, class HashFunc>
typename HashMap<K, T, HashFunc>::Iterator HashMap<K, T, HashFunc>::end()
{
	return Iterator(this, Iterator::SentinelTagInit::END);
}

template <class K, class T, class HashFunc>
typename HashMap<K, T, HashFunc>::ReverseIterator HashMap<K, T, HashFunc>::rEnd()
{
	Iterator iterator(this, Iterator::SentinelTagInit::BEGINNING);
	return ReverseIterator(iterator);
}

template <class K, class T, class HashFunc>
typename HashMap<K, T, HashFunc>::ConstIterator HashMap<K, T, HashFunc>::begin() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::BEGINNING);
	return ++iterator;
}

template <class K, class T, class HashFunc>
typename HashMap<K, T, HashFunc>::ConstReverseIterator HashMap<K, T, HashFunc>::rBegin() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::END);
	return ConstReverseIterator(--iterator);
}

template <class K, class T, class HashFunc>
typename HashMap<K, T, HashFunc>::ConstIterator HashMap<K, T, HashFunc>::end() const
{
	return ConstIterator(this, ConstIterator::SentinelTagInit::END);
}

template <class K, class T, class HashFunc>
typename HashMap<K, T, HashFunc>::ConstReverseIterator HashMap<K, T, HashFunc>::rEnd() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::BEGINNING);
	return ConstReverseIterator(iterator);
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
#if !NCINE_WITH_ALLOCATORS
	buffer_ = static_cast<uint8_t *>(::operator new(bytes));
	nodes_ = static_cast<Node *>(::operator new(sizeof(Node) * capacity_));
#else
	buffer_ = static_cast<uint8_t *>(alloc_.allocate(bytes));
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
#if !NCINE_WITH_ALLOCATORS
	buffer_ = static_cast<uint8_t *>(::operator new(bytes));
	nodes_ = static_cast<Node *>(::operator new(sizeof(Node) * capacity_));
#else
	buffer_ = static_cast<uint8_t *>(alloc_.allocate(bytes));
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
#if !NCINE_WITH_ALLOCATORS
		buffer_ = static_cast<uint8_t *>(::operator new(bytes));
		nodes_ = static_cast<Node *>(::operator new(sizeof(Node) * capacity_));
#else
		buffer_ = static_cast<uint8_t *>(alloc_.allocate(bytes));
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
template <class K, class T, class HashFunc>
bool HashMap<K, T, HashFunc>::insert(const K &key, const T &value)
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
template <class K, class T, class HashFunc>
bool HashMap<K, T, HashFunc>::insert(const K &key, T &&value)
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
template <class K, class T, class HashFunc>
template <typename... Args>
bool HashMap<K, T, HashFunc>::emplace(const K &key, Args &&... args)
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

template <class K, class T, class HashFunc>
void HashMap<K, T, HashFunc>::clear()
{
	destructNodes();
	initValues();
}

template <class K, class T, class HashFunc>
bool HashMap<K, T, HashFunc>::contains(const K &key, T &returnedValue) const
{
	int unsigned bucketIndex = 0;
	const bool found = findBucketIndex(key, bucketIndex);

	if (found)
		returnedValue = nodes_[bucketIndex].value;

	return found;
}

/*! \note Prefer this method if copying `T` is expensive, but always check the validity of returned pointer. */
template <class K, class T, class HashFunc>
T *HashMap<K, T, HashFunc>::find(const K &key)
{
	int unsigned bucketIndex = 0;
	const bool found = findBucketIndex(key, bucketIndex);

	T *returnedPtr = nullptr;
	if (found)
		returnedPtr = &nodes_[bucketIndex].value;

	return returnedPtr;
}

/*! \note Prefer this method if copying `T` is expensive, but always check the validity of returned pointer. */
template <class K, class T, class HashFunc>
const T *HashMap<K, T, HashFunc>::find(const K &key) const
{
	int unsigned bucketIndex = 0;
	const bool found = findBucketIndex(key, bucketIndex);

	const T *returnedPtr = nullptr;
	if (found)
		returnedPtr = &nodes_[bucketIndex].value;

	return returnedPtr;
}

/*! \return True if the element has been found and removed */
template <class K, class T, class HashFunc>
bool HashMap<K, T, HashFunc>::remove(const K &key)
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
				nodes_[bucketIndex].key = nctl::move(nodes_[lastBucketIndex].key);
				nodes_[bucketIndex].value = nctl::move(nodes_[lastBucketIndex].value);
				hashes_[bucketIndex] = hashes_[lastBucketIndex];
			}

			bucketIndex = lastBucketIndex;
		}

		hashes_[bucketIndex] = NullHash;
		destructObject(nodes_ + bucketIndex);
		size_--;
	}

	return found;
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
			const Node &node = nodes_[i];
			hashMap[node.key] = node.value;

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
bool HashMap<K, T, HashFunc>::findBucketIndex(const K &key, unsigned int &foundIndex, unsigned int &prevFoundIndex) const
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

template <class K, class T, class HashFunc>
bool HashMap<K, T, HashFunc>::findBucketIndex(const K &key, unsigned int &foundIndex) const
{
	unsigned int prevFoundIndex = 0;
	return findBucketIndex(key, foundIndex, prevFoundIndex);
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

template <class T>
using StringHashMap = HashMap<String, T, FNV1aHashFuncContainer<String>>;

template <class T>
using CStringHashMap = HashMap<const char *, T, FNV1aHashFunc<const char *>>;

}

#endif
