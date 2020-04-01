#ifndef CLASS_NCTL_STATICARRAY
#define CLASS_NCTL_STATICARRAY

#include <ncine/common_macros.h>
#include "ArrayIterator.h"
#include "ReverseIterator.h"
#include "utility.h"

namespace nctl {

/// Construction modes for the `StaticArray` class
/*! Declared outside the template class to use it without template parameters. */
enum class StaticArrayMode
{
	/// `StaticArray` will have a zero size
	ZERO_SIZE,
	/// `StaticArray` will extend the size to match its capacity
	EXTEND_SIZE
};

/// A static array based on templates that stores elements in the stack
template <class T, unsigned int C>
class StaticArray
{
  public:
	/// Iterator type
	using Iterator = ArrayIterator<T, false>;
	/// Constant iterator type
	using ConstIterator = ArrayIterator<T, true>;
	/// Reverse iterator type
	using ReverseIterator = nctl::ReverseIterator<Iterator>;
	/// Reverse constant iterator type
	using ConstReverseIterator = nctl::ReverseIterator<ConstIterator>;

	/// Constructs an empty array with fixed capacity
	StaticArray()
	    : size_(0), capacity_(C) {}
	/// Constructs an array with the option for it to have the size match its capacity
	explicit StaticArray(StaticArrayMode mode)
	    : size_(0), capacity_(C)
	{
		if (mode == StaticArrayMode::EXTEND_SIZE)
			size_ = capacity_;
	}

	/// Copy constructor
	StaticArray(const StaticArray &other);
	/// Move constructor
	StaticArray(StaticArray &&other);
	/// Assignment operator
	StaticArray &operator=(const StaticArray &other);
	/// Move assignment operator
	StaticArray &operator=(StaticArray &&other);

	/// Returns an iterator to the first element
	inline Iterator begin() { return Iterator(array_); }
	/// Returns a reverse iterator to the last element
	inline ReverseIterator rBegin() { return ReverseIterator(Iterator(array_ + size_ - 1)); }
	/// Returns an iterator to past the last element
	inline Iterator end() { return Iterator(array_ + size_); }
	/// Returns a reverse iterator to prior the first element
	inline ReverseIterator rEnd() { return ReverseIterator(Iterator(array_ - 1)); }

	/// Returns a constant iterator to the first element
	inline ConstIterator begin() const { return ConstIterator(array_); }
	/// Returns a constant reverse iterator to the last element
	inline ConstReverseIterator rBegin() const { return ConstReverseIterator(ConstIterator(array_ + size_ - 1)); }
	/// Returns a constant iterator to past the last lement
	inline ConstIterator end() const { return ConstIterator(array_ + size_); }
	/// Returns a constant reverse iterator to prior the first element
	inline ConstReverseIterator rEnd() const { return ConstReverseIterator(ConstIterator(array_ - 1)); }

	/// Returns a constant iterator to the first element
	inline ConstIterator cBegin() const { return ConstIterator(array_); }
	/// Returns a constant reverse iterator to the last element
	inline ConstReverseIterator crBegin() const { return ConstReverseIterator(ConstIterator(array_ + size_ - 1)); }
	/// Returns a constant iterator to past the last lement
	inline ConstIterator cEnd() const { return ConstIterator(array_ + size_); }
	/// Returns a constant reverse iterator to prior the first element
	inline ConstReverseIterator crEnd() const { return ConstReverseIterator(ConstIterator(array_ - 1)); }

	/// Returns true if the array is empty
	inline bool isEmpty() const { return size_ == 0; }
	/// Returns the array size
	/*! The array is filled without gaps until the `Size()`-1 element. */
	inline unsigned int size() const { return size_; }
	/// Returns the array capacity
	inline unsigned int capacity() const { return capacity_; }
	/// Sets a new size for the array (allowing for "holes")
	void setSize(unsigned int newSize);

	/// Clears the array
	/*! Size will be set to zero but capacity remains unmodified. */
	inline void clear() { size_ = 0; }
	/// Returns a constant reference to the first element in constant time
	inline const T &front() const { return array_[0]; }
	/// Returns a reference to the first element in constant time
	inline T &front() { return array_[0]; }
	/// Returns a constant reference to the last element in constant time
	inline const T &back() const { return array_[size_ - 1]; }
	/// Returns a reference to the last element in constant time
	inline T &back() { return array_[size_ - 1]; }
	/// Inserts a new element as the last one in constant time
	inline void pushBack(const T &element) { operator[](size_) = element; }
	/// Move inserts a new element as the last one in constant time
	inline void pushBack(T &&element) { operator[](size_) = nctl::move(element); }

	/// Removes an element at a specified position (moving the last element in place)
	int unorderedRemoveAt(unsigned int index);

	/// Read-only access to the specified element (with bounds checking)
	const T &at(unsigned int index) const;
	/// Access to the specified element (with bounds checking)
	T &at(unsigned int index);
	/// Read-only subscript operator
	const T &operator[](unsigned int index) const;
	/// Subscript operator
	T &operator[](unsigned int index);

	/// Returns a constant pointer to the allocated memory
	inline const T *data() const { return array_; }
	/// Returns a pointer to the allocated memory
	inline T *data() { return array_; }

  private:
	T array_[C];
	unsigned int size_;
	unsigned int capacity_;
};

template <class T, unsigned int C>
StaticArray<T, C>::StaticArray(const StaticArray<T, C> &other)
    : size_(other.size_), capacity_(other.capacity_)
{
	// copying all elements invoking their copy constructor
	for (unsigned int i = 0; i < other.size_; i++)
		array_[i] = other.array_[i];
}

template <class T, unsigned int C>
StaticArray<T, C>::StaticArray(StaticArray<T, C> &&other)
    : size_(other.size_), capacity_(other.capacity_)
{
	// moving all elements invoking their move constructor
	for (unsigned int i = 0; i < other.size_; i++)
		array_[i] = nctl::move(other.array_[i]);

	other.size_ = 0;
}

template <class T, unsigned int C>
StaticArray<T, C> &StaticArray<T, C>::operator=(const StaticArray<T, C> &other)
{
	size_ = other.size_;

	// copying all elements invoking their assignment operator
	for (unsigned int i = 0; i < other.size_; i++)
		array_[i] = other.array_[i];

	return *this;
}

template <class T, unsigned int C>
StaticArray<T, C> &StaticArray<T, C>::operator=(StaticArray<T, C> &&other)
{
	size_ = other.size_;

	// moving all elements invoking their move constructor
	for (unsigned int i = 0; i < other.size_; i++)
		array_[i] = nctl::move(other.array_[i]);

	other.size_ = 0;

	return *this;
}

template <class T, unsigned int C>
void StaticArray<T, C>::setSize(unsigned int newSize)
{
	if (newSize > capacity_)
	{
		LOGW_X("Trying to extend the size of a static array beyond its cpacity, from from %u to %u", capacity_, newSize);
		return;
	}

	size_ = newSize;
}

template <class T, unsigned int C>
int StaticArray<T, C>::unorderedRemoveAt(unsigned int index)
{
	// Cannot remove past the last element
	FATAL_ASSERT_MSG_X(index < size_, "Index %u out of size range", index);
	if (index < size_ - 1)
		array_[index] = nctl::move(array_[size_ - 1]);
	size_--;

	return index + 1;
}

template <class T, unsigned int C>
const T &StaticArray<T, C>::at(unsigned int index) const
{
	FATAL_ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
	return operator[](index);
}

template <class T, unsigned int C>
T &StaticArray<T, C>::at(unsigned int index)
{
	// Avoid creating "holes" into the array
	FATAL_ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);
	return operator[](index);
}

template <class T, unsigned int C>
const T &StaticArray<T, C>::operator[](unsigned int index) const
{
	ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
	return array_[index];
}

template <class T, unsigned int C>
T &StaticArray<T, C>::operator[](unsigned int index)
{
	// Avoid creating "holes" into the array
	ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);

	// Adding an element at the back of the array
	if (index == size_)
	{
		// Need growing
		if (size_ == capacity_)
			LOGW_X("Index %u is beyond capacity", index);
		else
			size_++;
	}

	return array_[index];
}

}

#endif
