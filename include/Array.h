#ifndef CLASS_NCINE_ARRAY
#define CLASS_NCINE_ARRAY

#include <cstdio> // for NULL
#include <cstring> // for memmove() and memcpy()
#include "common_macros.h"
#include "algorithms.h"
#include "ArrayIterator.h"

namespace ncine {

/// Construction modes for the `Array` class
/*! Declared outside the template class to use it without template parameters. */
struct ArrayMode
{
	enum Modes
	{
		/// `Array` will have a growing capacity
		GROWING_CAPACITY,
		/// `Array` will have a fixed capacity
		FIXED_CAPACITY
	};
};

/// A dynamic array based on templates that stores elements in the heap
template <class T>
class Array
{
  public:
	/// Iterator type
	typedef ArrayIterator<T, false> Iterator;
	/// Constant iterator type
	typedef ArrayIterator<T, true> ConstIterator;

	/// Default capacity for objects created by the default constructor
	static const unsigned int DefaultCapacity = 8;

	/// Default constructor
	Array()
		: array_(NULL), size_(0), capacity_(0), fixedCapacity_(false)
	{
		setCapacity(DefaultCapacity);
	}
	/// Constructs an array with explicit capacity
	explicit Array(unsigned int capacity)
		: array_(NULL), size_(0), capacity_(0), fixedCapacity_(false)
	{
		setCapacity(capacity);
	}
	/// Constructs an array with explicit capacity and the option for it to be fixed
	explicit Array(unsigned int capacity, ArrayMode::Modes mode)
		: array_(NULL), size_(0), capacity_(0),
		  fixedCapacity_(mode == ArrayMode::FIXED_CAPACITY)
	{
		setCapacity(capacity);
	}

	~Array() { delete[] array_; }

	/// Copy constructor
	Array(const Array &other);
	/// Copy-and-swap assignment operator
	Array &operator=(Array other);

	/// Swaps two arrays without copying their data
	void swap(Array &first, Array &second)
	{
		nc::swap(first.array_, second.array_);
		nc::swap(first.size_, second.size_);
		nc::swap(first.capacity_, second.capacity_);
		nc::swap(first.fixedCapacity_, second.fixedCapacity_);
	}

	/// Returns an iterator to the first element
	inline Iterator begin() { return Iterator(array_); }
	/// Returns an iterator to the last element
	inline Iterator rBegin() { return Iterator(array_ + size_ - 1); }
	/// Returns an iterator to past the last element
	inline Iterator end() { return Iterator(array_ + size_); }
	/// Returns an iterator to prior the first element
	inline Iterator rEnd() { return Iterator(array_ - 1); }

	/// Returns a constant iterator to the first element
	inline ConstIterator begin() const { return ConstIterator(array_); }
	/// Returns a constant iterator to the last element
	inline ConstIterator rBegin() const { return ConstIterator(array_ + size_ - 1); }
	/// Returns a constant iterator to past the last lement
	inline ConstIterator end() const { return ConstIterator(array_ + size_); }
	/// Returns a constant iterator to prior the first element
	inline ConstIterator rEnd() const { return ConstIterator(array_ - 1); }

	/// Returns true if the array is empty
	inline bool isEmpty() const { return size_ == 0; }
	/// Returns the array size
	/*! The array is filled without gaps until the `Size()`-1 element. */
	inline unsigned int size() const { return size_; }
	/// Returns the array capacity
	/*! The array has memory allocated to store until the `Capacity()`-1 element. */
	inline unsigned int capacity() const { return capacity_; }
	/// Sets a new capacity for the array (can be bigger or smaller than the current one)
	void setCapacity(unsigned int newCapacity);
	/// Sets a new size for the array (allowing for "holes")
	void setSize(unsigned int newSize);
	/// Decreases the capacity to match the current size of the array
	void shrinkToFit();

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
	inline void pushBack(T element) { operator[](size_) = element; }
	/// Removes the last element in constant time
	void popBack();
	/// Inserts new elements at the specified position from a source range, last not included (shifting elements around)
	void insertRange(unsigned int index, const T *firstPtr, const T *lastPtr);
	/// Inserts a new element at a specified position (shifting elements around)
	void insertAt(unsigned int index, T element);
	/// Inserts a new element at the position specified by the iterator (shifting elements around)
	Iterator insert(Iterator position, const T &value);
	/// Inserts new elements from a source at the position specified by the iterator (shifting elements around)
	Iterator insert(Iterator position, Iterator first, Iterator last);
	/// Removes the specified range of elements, last not included (shifting elements around)
	void removeRange(unsigned int firstIndex, unsigned int lastIndex);
	/// Removes an element at a specified position (shifting elements around)
	inline void removeAt(unsigned int index) { removeRange(index, index + 1); }
	/// Removes the element pointed by the iterator (shifting elements around)
	Iterator erase(Iterator position);
	/// Removes the elements in the range, last not included (shifting elements around)
	Iterator erase(Iterator first, const Iterator last);
	/// Inserts an array of elements at the end in constant time
	void append(const T *elements, unsigned int amount);

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
	/*! When adding new elements through a pointer the size field is not updated, like with `std::vector`. */
	inline T *data() { return array_; }

  private:
	T *array_;
	unsigned int size_;
	unsigned int capacity_;
	bool fixedCapacity_;
};

template <class T>
Array<T>::Array(const Array<T> &other)
	: array_(NULL), size_(other.size_), capacity_(other.capacity_), fixedCapacity_(other.fixedCapacity_)
{
	array_ = new T[capacity_];
	for (unsigned int i = 0; i < size_; i++)
	{
		// copying all elements invoking their copy constructor
		array_[i] = other.array_[i];
	}
}

/*! The parameter should be passed by value for the idiom to work. */
template <class T>
Array<T> &Array<T>::operator=(Array<T> other)
{
	swap(*this, other);
	return *this;
}

template <class T>
void Array<T>::setCapacity(unsigned int newCapacity)
{
	FATAL_ASSERT_MSG(newCapacity > 0, "Zero is not a valid capacity");

	// If the call does not come from the constructor
	if (capacity_ != 0)
	{
		// Setting a new capacity is disabled if the array is fixed
		if (fixedCapacity_)
		{
			LOGW_X("Trying to change the capacity of a fixed array, from from %u to %u", capacity_, newCapacity);
			return;
		}

		if (newCapacity == capacity_)
		{
			LOGW_X("Array capacity already equal to %u", capacity_);
			return;
		}
		else if (newCapacity > capacity_)
		{
			// LOGW_X("Array capacity growing from %u to %u", capacity_, newCapacity);
		}
		else if (newCapacity < capacity_)
			LOGW_X("Array capacity shrinking from %u to %u", capacity_, newCapacity);
	}

	T *newArray = new T[newCapacity];

	if (size_ > 0)
	{
		if (newCapacity < size_) // shrinking
			size_ = newCapacity; // cropping last elements

		memcpy(newArray, array_, sizeof(T) * size_);
	}

	delete[] array_;
	array_ = newArray;
	capacity_ = newCapacity;
}

template <class T>
void Array<T>::setSize(unsigned int newSize)
{
	if (newSize > capacity_)
		setCapacity(newSize);

	size_ = newSize;
}

template <class T>
void Array<T>::shrinkToFit()
{
	setCapacity(size_);
}

template <class T>
void Array<T>::popBack()
{
	if (size_ > 0)
		size_--;
}

template <class T>
void Array<T>::insertRange(unsigned int index, const T *firstPtr, const T *lastPtr)
{
	// Cannot insert at more than one position after the last element
	FATAL_ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);
	FATAL_ASSERT_MSG_X(firstPtr < lastPtr, "First pointer %p should precede the last one %p", firstPtr, lastPtr);

	if (size_ + 1 > capacity_)
		setCapacity(size_ * 2);

	// memmove() takes care of overlapping regions
	memmove(array_ + index, firstPtr, sizeof(T) * (lastPtr - firstPtr));
	size_ += static_cast<unsigned int>(lastPtr - firstPtr);
}

template <class T>
void Array<T>::insertAt(unsigned int index, T element)
{
	// Cannot insert at more than one position after the last element
	FATAL_ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);

	if (size_ + 1 > capacity_)
		setCapacity(size_ * 2);

	// memmove() takes care of overlapping regions
	memmove(array_ + index + 1, array_ + index, sizeof(T) * (size_ - index));
	array_[index] = element;
	size_++;
}

template <class T>
typename Array<T>::Iterator Array<T>::insert(Iterator position, const T &value)
{
	const unsigned int index = &(*position) - array_;
	insertAt(index, value);

	return ++position;
}

template <class T>
typename Array<T>::Iterator Array<T>::insert(Iterator position, Iterator first, Iterator last)
{
	const unsigned int index = static_cast<unsigned int>(&(*position) - array_);
	const T *firstPtr = &(*first);
	const T *lastPtr = &(*last);
	insertRange(index, firstPtr, lastPtr);

	return position + static_cast<unsigned int>(lastPtr - firstPtr);
}

template <class T>
void Array<T>::removeRange(unsigned int firstIndex, unsigned int lastIndex)
{
	// Cannot remove past the last element
	FATAL_ASSERT_MSG_X(firstIndex < size_, "First index %u out of size range", firstIndex);
	FATAL_ASSERT_MSG_X(lastIndex <= size_, "Last index %u out of size range", lastIndex);
	FATAL_ASSERT_MSG_X(firstIndex < lastIndex, "First index %u should be less than last index %u", firstIndex, lastIndex);

	// memmove() takes care of overlapping regions
	memmove(array_ + firstIndex, array_ + lastIndex, sizeof(T) * (size_ - lastIndex));
	size_ -= (lastIndex - firstIndex);
}

template <class T>
typename Array<T>::Iterator Array<T>::erase(Iterator position)
{
	const unsigned int index = static_cast<unsigned int>(&(*position) - array_);
	removeAt(index);

	return ++position;
}

template <class T>
typename Array<T>::Iterator Array<T>::erase(Iterator first, const Iterator last)
{
	const unsigned int firstIndex = static_cast<unsigned int>(&(*first) - array_);
	const unsigned int lastIndex = static_cast<unsigned int>(&(*last) - array_);
	removeRange(firstIndex, lastIndex);

	return ++first;
}

template <class T>
void Array<T>::append(const T *elements, unsigned int amount)
{
	if (size_ + amount > capacity_)
		setCapacity(size_ + amount);

	memcpy(array_ + size_, elements, sizeof(T) * amount);
	size_ += amount;
}

template <class T>
const T &Array<T>::at(unsigned int index) const
{
	FATAL_ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
	return operator[](index);
}

template <class T>
T &Array<T>::at(unsigned int index)
{
	// Avoid creating "holes" into the array
	FATAL_ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);
	return operator[](index);
}

template <class T>
const T &Array<T>::operator[](unsigned int index) const
{
	ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
	return array_[index];
}

template <class T>
T &Array<T>::operator[](unsigned int index)
{
	// Avoid creating "holes" into the array
	ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);

	// Adding an element at the back of the array
	if (index == size_)
	{
		// Need growing
		if (size_ == capacity_)
			setCapacity(capacity_ * 2);

		size_++;
	}

	return array_[index];
}

}

#endif
