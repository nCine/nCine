#ifndef CLASS_NCTL_STATICHASHMAP
#define CLASS_NCTL_STATICHASHMAP

#include <new>
#include <initializer_list>
#include <ncine/common_macros.h>
#include "HashFunctions.h"
#include "Pair.h"
#include "ReverseIterator.h"

namespace nctl {

template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst> class StaticHashMapIterator;
template <class K, class T, class HashFunc, unsigned int Capacity, bool IsConst> struct StaticHashMapHelperTraits;
class String;

/// A template based hashmap implementation with open addressing and leapfrog probing (version with static allocation)
template <class K, class T, unsigned int Capacity, class HashFunc = FastHashFunc<K>>
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

	/// Constructs an empty hashmap
	inline StaticHashMap()
	    : size_(0) { init(); }
	/// Constructs an hashmap with an initializer list
	inline StaticHashMap(std::initializer_list<Pair<K, T>> initList)
	    : StaticHashMap() { insert(initList); }
	inline ~StaticHashMap() { destructNodes(); }

	/// Copy constructor
	StaticHashMap(const StaticHashMap &other);
	/// Move constructor
	StaticHashMap(StaticHashMap &&other);
	/// Aassignment operator
	StaticHashMap &operator=(const StaticHashMap &other);
	/// Move aassignment operator
	StaticHashMap &operator=(StaticHashMap &&other);

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
	T *find(const K &key);
	/// Checks whether an element is in the hashmap or not (read-only)
	const T *find(const K &key) const;
	/// Checks whether an element is in the hashmap or not
	bool contains(const K &key) const;
	/// Removes a key from the hashmap, if it exists
	bool remove(const K &key);

  private:
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

	unsigned int size_;
	uint8_t delta1_[Capacity];
	uint8_t delta2_[Capacity];
	hash_t hashes_[Capacity];
	uint8_t nodesBuffer_[Capacity * sizeof(Node)];
	Node *nodes_ = reinterpret_cast<Node *>(nodesBuffer_);
	HashFunc hashFunc_;

	void init();
	void destructNodes();

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
typename StaticHashMap<K, T, Capacity, HashFunc>::Iterator StaticHashMap<K, T, Capacity, HashFunc>::end()
{
	return Iterator(this, Iterator::SentinelTagInit::END);
}

template <class K, class T, unsigned int Capacity, class HashFunc>
inline typename StaticHashMap<K, T, Capacity, HashFunc>::ConstIterator StaticHashMap<K, T, Capacity, HashFunc>::begin() const
{
	ConstIterator iterator(this, ConstIterator::SentinelTagInit::BEGINNING);
	return ++iterator;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
inline typename StaticHashMap<K, T, Capacity, HashFunc>::ConstIterator StaticHashMap<K, T, Capacity, HashFunc>::end() const
{
	return ConstIterator(this, ConstIterator::SentinelTagInit::END);
}

template <class K, class T, unsigned int Capacity, class HashFunc>
StaticHashMap<K, T, Capacity, HashFunc>::StaticHashMap(const StaticHashMap<K, T, Capacity, HashFunc> &other)
    : size_(other.size_)
{
	for (unsigned int i = 0; i < Capacity; i++)
	{
		if (other.hashes_[i] != NullHash)
			new (nodes_ + i) Node(other.nodes_[i]);

		delta1_[i] = other.delta1_[i];
		delta2_[i] = other.delta2_[i];
		hashes_[i] = other.hashes_[i];
	}
}

template <class K, class T, unsigned int Capacity, class HashFunc>
StaticHashMap<K, T, Capacity, HashFunc>::StaticHashMap(StaticHashMap<K, T, Capacity, HashFunc> &&other)
    : size_(other.size_)
{
	for (unsigned int i = 0; i < Capacity; i++)
	{
		if (other.hashes_[i] != NullHash)
			new (nodes_ + i) Node(nctl::move(other.nodes_[i]));

		delta1_[i] = other.delta1_[i];
		delta2_[i] = other.delta2_[i];
		hashes_[i] = other.hashes_[i];
	}
	other.destructNodes();
}

template <class K, class T, unsigned int Capacity, class HashFunc>
StaticHashMap<K, T, Capacity, HashFunc> &StaticHashMap<K, T, Capacity, HashFunc>::operator=(const StaticHashMap<K, T, Capacity, HashFunc> &other)
{
	if (this == &other)
		return *this;

	for (unsigned int i = 0; i < Capacity; i++)
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

template <class K, class T, unsigned int Capacity, class HashFunc>
StaticHashMap<K, T, Capacity, HashFunc> &StaticHashMap<K, T, Capacity, HashFunc>::operator=(StaticHashMap<K, T, Capacity, HashFunc> &&other)
{
	if (this == &other)
		return *this;

	for (unsigned int i = 0; i < Capacity; i++)
	{
		if (other.hashes_[i] != NullHash)
		{
			if (hashes_[i] != NullHash)
				nodes_[i] = nctl::move(other.nodes_[i]);
			else
				new (nodes_ + i) Node(nctl::move(other.nodes_[i]));
		}
		else if (hashes_[i] != NullHash)
			destructObject(nodes_ + i);

		delta1_[i] = other.delta1_[i];
		delta2_[i] = other.delta2_[i];
		hashes_[i] = other.hashes_[i];
	}
	size_ = other.size_;
	other.destructNodes();

	return *this;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
T &StaticHashMap<K, T, Capacity, HashFunc>::operator[](const K &key)
{
	const hash_t hash = hashFunc_(key);
	const ProbeResult r = probe(key, hash);

	if (r.foundFlag)
		return nodes_[r.found].value;

	const unsigned int index = patchDeltas(r);
	return addNode(index, hash, key);
}

/*! \return True if all initializer list elements have been inserted */
template <class K, class T, unsigned int Capacity, class HashFunc>
bool StaticHashMap<K, T, Capacity, HashFunc>::insert(std::initializer_list<Pair<K, T>> initList)
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
template <class K, class T, unsigned int Capacity, class HashFunc>
template <typename... Args>
bool StaticHashMap<K, T, Capacity, HashFunc>::emplace(const K &key, Args &&... args)
{
	const hash_t hash = hashFunc_(key);
	const ProbeResult r = probe(key, hash);

	if (r.foundFlag)
		return false;

	const unsigned int index = patchDeltas(r);
	emplaceNode(index, hash, key, nctl::forward<Args>(args)...);
	return true;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
void StaticHashMap<K, T, Capacity, HashFunc>::clear()
{
	destructNodes();
	init();
}

/*! \note Prefer this method if copying `T` is expensive, but always check the validity of returned pointer. */
template <class K, class T, unsigned int Capacity, class HashFunc>
T *StaticHashMap<K, T, Capacity, HashFunc>::find(const K &key)
{
	const hash_t hash = hashFunc_(key);
	const ProbeResult r = probe(key, hash);
	return (r.foundFlag) ? &nodes_[r.found].value : nullptr;
}

/*! \note Prefer this method if copying `T` is expensive, but always check the validity of returned pointer. */
template <class K, class T, unsigned int Capacity, class HashFunc>
const T *StaticHashMap<K, T, Capacity, HashFunc>::find(const K &key) const
{
	const hash_t hash = hashFunc_(key);
	const ProbeResult r = probe(key, hash);
	return (r.foundFlag) ? &nodes_[r.found].value : nullptr;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
bool StaticHashMap<K, T, Capacity, HashFunc>::contains(const K &key) const
{
	return find(key) != nullptr;
}

/*! \return True if the element has been found and removed */
template <class K, class T, unsigned int Capacity, class HashFunc>
bool StaticHashMap<K, T, Capacity, HashFunc>::remove(const K &key)
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
	destructObject(nodes_ + bucketIndex);
	size_--;

	return true;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
void StaticHashMap<K, T, Capacity, HashFunc>::init()
{
	for (unsigned int i = 0; i < Capacity; i++)
		delta1_[i] = 0;
	for (unsigned int i = 0; i < Capacity; i++)
		delta2_[i] = 0;
	for (unsigned int i = 0; i < Capacity; i++)
		hashes_[i] = NullHash;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
void StaticHashMap<K, T, Capacity, HashFunc>::destructNodes()
{
	for (unsigned int i = 0; i < Capacity; i++)
	{
		if (hashes_[i] != NullHash)
		{
			destructObject(nodes_ + i);
			hashes_[i] = NullHash;
		}
	}
	size_ = 0;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
typename StaticHashMap<K, T, Capacity, HashFunc>::ProbeResult
StaticHashMap<K, T, Capacity, HashFunc>::probe(const K &key, hash_t hash) const
{
	ProbeResult r{};
	r.ideal = hash % Capacity;
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

template <class K, class T, unsigned int Capacity, class HashFunc>
unsigned int StaticHashMap<K, T, Capacity, HashFunc>::patchDeltas(const ProbeResult &r)
{
	if (r.ideal == r.empty)
		return r.empty;

	if (delta1_[r.ideal] == 0)
		delta1_[r.ideal] = calcNewDelta(r.ideal, r.empty);
	else // r.prev must be the tail
		delta2_[r.prev] = calcNewDelta(r.prev, r.empty);

	return r.empty;
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
	return (hashes_[index] == NullHash || (hashes_[index] == hash && equalTo(nodes_[index].key, key)));
}

template <class K, class T, unsigned int Capacity, class HashFunc>
bool StaticHashMap<K, T, Capacity, HashFunc>::bucketFound(unsigned int index, hash_t hash, const K &key) const
{
	return (hashes_[index] == hash && equalTo(nodes_[index].key, key));
}

template <class K, class T, unsigned int Capacity, class HashFunc>
template <class ValueArg>
bool StaticHashMap<K, T, Capacity, HashFunc>::insertImpl(const K &key, ValueArg &&value)
{
	const hash_t hash = hashFunc_(key);
	const ProbeResult r = probe(key, hash);

	if (r.foundFlag)
		return false;

	const unsigned int index = patchDeltas(r);
	insertNode(index, hash, key, nctl::forward<ValueArg>(value));
	return true;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
T &StaticHashMap<K, T, Capacity, HashFunc>::addNode(unsigned int index, hash_t hash, const K &key)
{
	FATAL_ASSERT(size_ < Capacity);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	new (nodes_ + index) Node(key);

	return nodes_[index].value;
}

template <class K, class T, unsigned int Capacity, class HashFunc>
void StaticHashMap<K, T, Capacity, HashFunc>::insertNode(unsigned int index, hash_t hash, const K &key, const T &value)
{
	FATAL_ASSERT(size_ < Capacity);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	new (nodes_ + index) Node(key, value);
}

template <class K, class T, unsigned int Capacity, class HashFunc>
void StaticHashMap<K, T, Capacity, HashFunc>::insertNode(unsigned int index, hash_t hash, const K &key, T &&value)
{
	FATAL_ASSERT(size_ < Capacity);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	new (nodes_ + index) Node(key, nctl::move(value));
}

template <class K, class T, unsigned int Capacity, class HashFunc>
template <typename... Args>
void StaticHashMap<K, T, Capacity, HashFunc>::emplaceNode(unsigned int index, hash_t hash, const K &key, Args &&... args)
{
	FATAL_ASSERT(size_ < Capacity);
	FATAL_ASSERT(hashes_[index] == NullHash);

	size_++;
	hashes_[index] = hash;
	new (nodes_ + index) Node(key, nctl::forward<Args>(args)...);
}

}

#endif
