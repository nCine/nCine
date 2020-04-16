#ifndef CLASS_NCTL_SPARSESET
#define CLASS_NCTL_SPARSESET

#include <ncine/common_macros.h>
#include "ReverseIterator.h"
#include <cstring> // for memcpy()

#include <ncine/config.h>
#if NCINE_WITH_ALLOCATORS
	#include "AllocManager.h"
	#include "IAllocator.h"
#endif

namespace nctl {

template <class T> class SparseSetIterator;

/// A template based sparse set implementation
template <class T>
class SparseSet
{
  public:
	static const T NotFound = T(-1);

	/// Iterator type
	/*! Elements in the SparseSet can never be changed */
	using Iterator = SparseSetIterator<T>;
	/// Constant iterator type
	using ConstIterator = SparseSetIterator<T>;
	/// Reverse iterator type
	using ReverseIterator = nctl::ReverseIterator<Iterator>;
	/// Reverse constant iterator type
	using ConstReverseIterator = nctl::ReverseIterator<ConstIterator>;

	SparseSet(unsigned int capacity, unsigned int maxValue);
#if NCINE_WITH_ALLOCATORS
	SparseSet(unsigned int capacity, unsigned int maxValue, IAllocator &alloc);
#endif
	~SparseSet();

	/// Copy constructor
	SparseSet(const SparseSet &other);
	/// Move constructor
	SparseSet(SparseSet &&other);
	/// Assignment operator
	SparseSet &operator=(const SparseSet &other);
	/// Move assignment operator
	SparseSet &operator=(SparseSet &&other);

	/// Swaps two SparseSets without copying their data
	inline void swap(SparseSet &first, SparseSet &second)
	{
#if NCINE_WITH_ALLOCATORS
		nctl::swap(first.alloc_, second.alloc_);
#endif
		nctl::swap(first.size_, second.size_);
		nctl::swap(first.capacity_, second.capacity_);
		nctl::swap(first.maxValue_, second.maxValue_);
		nctl::swap(first.sparse_, second.sparse_);
		nctl::swap(first.dense_, second.dense_);
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
	bool insert(T value);

	/// Returns the maximum value of the sparseset
	inline unsigned int maxValue() const { return maxValue_; }
	/// Returns the capacity of the sparseset
	inline unsigned int capacity() const { return capacity_; }
	/// Returns true if the sparseset is empty
	inline bool isEmpty() const { return size_ == 0; }
	/// Returns the number of elements in the sparseset
	inline unsigned int size() const { return size_; }
	/// Returns the ratio between used and total buckets
	inline float loadFactor() const { return size_ / static_cast<float>(capacity_); }

	/// Clears the sparseset
	inline void clear() { size_ = 0; }
	/// Checks whether an element is in the sparseset or not
	bool contains(T value) const;
	/// Checks whether an element is in the sparseset or not
	T find(T value) const;
	/// Removes a key from the sparseset, if it exists
	bool remove(T value);

	/// Sets the number of buckets to the new specified size and rehashes the container
	void rehash(unsigned int count);

  private:
#if NCINE_WITH_ALLOCATORS
	/// The custom memory allocator for the sparseset
	IAllocator &alloc_;
#endif
	unsigned int size_;
	unsigned int capacity_;
	T maxValue_;
	T *sparse_;
	T *dense_;

	friend class SparseSetIterator<T>;
};

template <class T>
inline typename SparseSet<T>::ConstIterator SparseSet<T>::begin()
{
	ConstIterator iterator(this, 0);
	return iterator;
}

template <class T>
typename SparseSet<T>::ConstReverseIterator SparseSet<T>::rBegin()
{
	ConstIterator iterator(this, size_ - 1);
	return ConstReverseIterator(iterator);
}

template <class T>
typename SparseSet<T>::ConstIterator SparseSet<T>::end()
{
	return ConstIterator(this, size_);
}

template <class T>
typename SparseSet<T>::ConstReverseIterator SparseSet<T>::rEnd()
{
	ConstIterator iterator(this, -1);
	return ConstReverseIterator(iterator);
}

template <class T>
typename SparseSet<T>::ConstIterator SparseSet<T>::begin() const
{
	ConstIterator iterator(this, 0);
	return iterator;
}

template <class T>
typename SparseSet<T>::ConstReverseIterator SparseSet<T>::rBegin() const
{
	ConstIterator iterator(this, size_ - 1);
	return ConstReverseIterator(iterator);
}

template <class T>
typename SparseSet<T>::ConstIterator SparseSet<T>::end() const
{
	return ConstIterator(this, size_);
}

template <class T>
typename SparseSet<T>::ConstReverseIterator SparseSet<T>::rEnd() const
{
	ConstIterator iterator(this, -1);
	return ConstReverseIterator(iterator);
}

template <class T>
SparseSet<T>::SparseSet(unsigned int capacity, unsigned int maxValue)
    :
#if NCINE_WITH_ALLOCATORS
      alloc_(theDefaultAllocator()),
#endif
      size_(0), capacity_(capacity), maxValue_(maxValue),
      sparse_(nullptr), dense_(nullptr)
{
	FATAL_ASSERT_MSG(capacity > 0, "Zero is not a valid capacity");
	FATAL_ASSERT(maxValue + 1 >= capacity);

#if !NCINE_WITH_ALLOCATORS
	sparse_ = new T[maxValue + 1];
	dense_ = new T[capacity];
#else
	sparse_ = alloc_.newArray<T>(maxValue + 1);
	dense_ = alloc_.newArray<T>(capacity);
#endif
}

#if NCINE_WITH_ALLOCATORS
template <class T>
SparseSet<T>::SparseSet(unsigned int capacity, unsigned int maxValue, IAllocator &alloc)
    : alloc_(alloc), size_(0), capacity_(capacity), maxValue_(maxValue),
      sparse_(nullptr), dense_(nullptr)
{
	static_assert(nctl::isIntegral<T>::value, "Integral type is required");
	FATAL_ASSERT_MSG(capacity > 0, "Zero is not a valid capacity");
	FATAL_ASSERT(maxValue + 1 >= capacity);

	sparse_ = alloc_.newArray<T>(maxValue + 1);
	dense_ = alloc_.newArray<T>(capacity);
}
#endif

template <class T>
SparseSet<T>::~SparseSet()
{
#if !NCINE_WITH_ALLOCATORS
	delete[] sparse_;
	delete[] dense_;
#else
	alloc_.deleteArray<T>(sparse_);
	alloc_.deleteArray<T>(dense_);
#endif
}

template <class T>
SparseSet<T>::SparseSet(const SparseSet<T> &other)
    :
#if NCINE_WITH_ALLOCATORS
      alloc_(other.alloc_),
#endif
      size_(other.size_), capacity_(other.capacity_), maxValue_(other.maxValue_)
{
#if !NCINE_WITH_ALLOCATORS
	sparse_ = new T[maxValue_ + 1];
	dense_ = new T[capacity_];
#else
	sparse_ = alloc_.newArray<T>(maxValue_ + 1);
	dense_ = alloc_.newArray<T>(capacity_);
#endif

	memcpy(sparse_, other.sparse_, (maxValue_ + 1) * sizeof(T));
	memcpy(dense_, other.dense_, capacity_ * sizeof(T));
}

template <class T>
SparseSet<T>::SparseSet(SparseSet<T> &&other)
    :
#if NCINE_WITH_ALLOCATORS
      alloc_(other.alloc_),
#endif
      size_(other.size_), capacity_(other.capacity_), maxValue_(other.maxValue_),
      sparse_(other.sparse_), dense_(other.dense_)
{
	other.size_ = 0;
	other.capacity_ = 0;
	other.maxValue_ = 0;
	other.sparse_ = nullptr;
	other.dense_ = nullptr;
}

template <class T>
SparseSet<T> &SparseSet<T>::operator=(const SparseSet<T> &other)
{
	if (this == &other)
		return *this;

	if (other.maxValue_ > maxValue_)
	{
		maxValue_ = other.maxValue_;
#if !NCINE_WITH_ALLOCATORS
		delete[] sparse_;
		sparse_ = new T[maxValue_ + 1];
#else
		alloc_.deleteArray<T>(sparse_);
		sparse_ = alloc_.newArray<T>(maxValue_ + 1);
#endif
	}

	if (other.size_ > capacity_)
	{
		capacity_ = other.size_;
#if !NCINE_WITH_ALLOCATORS
		delete[] dense_;
		dense_ = new T[capacity_];
#else
		alloc_.deleteArray<T>(dense_);
		dense_ = alloc_.newArray<T>(capacity_);
#endif
	}

	memcpy(sparse_, other.sparse_, maxValue_ * sizeof(T));
	memcpy(dense_, other.dense_, capacity_ * sizeof(T));
	size_ = other.size_;

	return *this;
}

template <class T>
SparseSet<T> &SparseSet<T>::operator=(SparseSet<T> &&other)
{
	if (this != &other)
	{
		swap(*this, other);
		other.clear();
	}
	return *this;
}

/*! \return True if the element has been inserted */
template <class T>
bool SparseSet<T>::insert(T value)
{
	if (value > maxValue_ || size_ >= capacity_ || contains(value) == true)
		return false;

	dense_[size_] = value;
	sparse_[value] = size_;
	size_++;

	return true;
}

template <class T>
bool SparseSet<T>::contains(T value) const
{
	if (size_ == 0 || value > maxValue_)
		return false;

	if (sparse_[value] < size_ && dense_[sparse_[value]] == value)
		return true;

	return false;
}

template <class T>
T SparseSet<T>::find(T value) const
{
	if (size_ == 0 || value > maxValue_)
		return NotFound;

	if (sparse_[value] < size_ && dense_[sparse_[value]] == value)
		return sparse_[value];

	return NotFound;
}

/*! \return True if the element has been found and removed */
template <class T>
bool SparseSet<T>::remove(T value)
{
	if (contains(value) == false)
		return false;

	T temp = dense_[size_ - 1];
	dense_[sparse_[value]] = temp;
	sparse_[temp] = sparse_[value];
	size_--;

	return true;
}

template <class T>
void SparseSet<T>::rehash(unsigned int count)
{
	if (size_ == 0 || count < size_)
		return;

	SparseSet<T> sparseSet(count, maxValue_);

	for (unsigned int i = 0; i < size_; i++)
		sparseSet.insert(dense_[i]);

	*this = nctl::move(sparseSet);
}

}

#endif
