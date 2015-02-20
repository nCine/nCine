#ifndef CLASS_NCINE_ARRAY
#define CLASS_NCINE_ARRAY

#include <cstdio> // for NULL
#include <cstdlib> // for exit()
#include <cstring> // for memmove() and memcpy()
#include "common.h"
#include "ArrayIterator.h"
#include "ServiceLocator.h"

namespace ncine {

/// A dynamic array based on templates and stored in the heap
template <class T>
class Array
{
  public:
	/// Iterator type
	typedef ArrayIterator<T> Iterator;
	/// Constant iterator type
	typedef const ArrayIterator<T> Const_Iterator;

	/// Constructs an array with explicit capacity
	explicit Array(unsigned int capacity)
		: array_(NULL), size_(0), capacity_(0), fixedCapacity_(false)
	{
		setCapacity(capacity);
	}
	/// Constructs an array with explicit capacity and the option for it to be fixed
	explicit Array(unsigned int capacity, bool fixedCapacity)
		: array_(NULL), size_(0), capacity_(0), fixedCapacity_(fixedCapacity)
	{
		setCapacity(capacity);
	}

	~Array() { delete[] array_; }

	// Copy constructor
	Array(const Array& other);
	// Copy-and-swap assignment operator
	Array& operator=(Array other);

	/// Swaps two arrays without copying their data
	void swap(Array& first, Array& second)
	{
		nc::swap(first.array_, second.array_);
		nc::swap(first.size_, second.size_);
		nc::swap(first.capacity_, second.capacity_);
	}

	/// Returns an iterator to the first character
	inline Iterator begin() { return Iterator(array_); }
	/// Returns an iterator to the last character
	inline Iterator rBegin() { return Iterator(array_ + size_ - 1); }
	/// Returns an iterator to the termination character
	inline Iterator end() { return Iterator(array_ + size_); }
	/// Returns an iterator to the byte preceding the first character
	inline Iterator rEnd() { return Iterator(array_ - 1); }

	/// Returns a constant iterator to the first character
	inline Const_Iterator begin() const { return Iterator(array_); }
	/// Returns a constant iterator to the last character
	inline Const_Iterator rBegin() const { return Iterator(array_ + size_ - 1); }
	/// Returns a constant iterator to the termination character
	inline Const_Iterator end() const { return Iterator(array_ + size_); }
	/// Returns a constant iterator to the byte preceding the first character
	inline Const_Iterator rEnd() const { return Iterator(array_ - 1); }

	/// Returns true if the array is empty
	inline bool isEmpty() const { return size_ == 0; }
	/// Returns the array size
	/** The array is filled without gaps until the Size()-1 element */
	inline unsigned int size() const { return size_; }
	/// Returns the array capacity
	/// The array has memory allocated to store until the Capacity()-1 element
	inline unsigned int capacity() const { return capacity_; }
	// Set a new capacity for the array (can be bigger or smaller than the current one)
	void setCapacity(unsigned int newCapacity);

	/// Clears the array
	/** Size will be zero but capacity remains untouched */
	inline void clear() { size_ = 0; }
	/// Inserts a new element as the last one in constant time
	inline void insertBack(T element) { operator[](size_) = element; }
	// Inserts a new element at a specified position (shifting elements around)
	void insertAt(unsigned int index, T element);
	// Removes an element at a specified position (shifting elements around)
	void removeAt(unsigned int index);
	// Inserts an array of elements at the end in constant time
	void append(const T* elements, unsigned int amount);

	// Read-only subscript operator
	const T& operator[](unsigned int index) const;
	// Subscript operator
	T& operator[](unsigned int index);

	/// Returns a pointer to the allocated memory
	/** When adding new elements through a pointer the size field is not updated, like with std::vector */
	inline T* data() { return array_; }
	/// Returns a constant pointer to the allocated memory
	/** It's useful when holding arrays of OpenGL data */
	inline const T* data() const { return array_; }
	// Allows for direct but unchecked access to the array memory
	T* mapBuffer(unsigned int reserved);

  private:
	T* array_;
	unsigned int size_;
	unsigned int capacity_;
	bool fixedCapacity_;
};

/// Copy constructor
template <class T>
Array<T>::Array(const Array<T>& other)
	: array_(NULL), size_(other.size_), capacity_(other.capacity_)
{
	array_ = new T[capacity_];
	memcpy(array_, other.array_, sizeof(T) * size_);
}

/// Copy-and-swap assignment operator
/** The parameter should be passed by value for the idiom to work */
template <class T>
Array<T>& Array<T>::operator=(Array<T> other)
{
	swap(*this, other);
	return *this;
}

/// Sets a new capacity for the array (can be bigger or smaller than the current one)
template <class T>
void Array<T>::setCapacity(unsigned int newCapacity)
{
	// If the call does not come from the constructor
	if (capacity_ != 0)
	{
		// Setting a new capacity is disabled if the array is fixed
		if (fixedCapacity_)
		{
			LOGW_X("Trying to change the capacity of a fixed array, from from %u to %u", capacity_, newCapacity);
			return;
		}

		if (newCapacity == 0)
		{
			LOGF("Zero is not a valid capacity");
			exit(EXIT_FAILURE);
		}
		else if (newCapacity == capacity_)
		{
			LOGW_X("Array capacity already equal to %u", capacity_);
			return;
		}
		else if (newCapacity > capacity_)
		{
			//LOGW_X("Array capacity growing from %u to %u", capacity_, newCapacity);
		}
		else if (newCapacity < capacity_)
		{
			LOGW_X("Array capacity shrinking from %u to %u", capacity_, newCapacity);
		}
	}

	T* newArray = new T[newCapacity];

	if (size_ > 0)
	{
		if (newCapacity < size_) // shrinking
		{
			size_ = newCapacity;    // cropping last elements
		}

		memcpy(newArray, array_, sizeof(T)*size_);
	}

	delete[] array_;
	array_ = newArray;
	capacity_ = newCapacity;
}

/// Inserts a new element at a specified position (shifting elements around)
template <class T>
void Array<T>::insertAt(unsigned int index, T element)
{
	// Cannot insert at more than one position after the last element
	if (index > size_)
	{
		LOGF_X("Element %u out of size range", index);
		exit(EXIT_FAILURE);
	}

	if (size_ + 1 > capacity_)
	{
		setCapacity(size_ * 2);
	}

	// memmove() takes care of overlapping regions
	memmove(array_ + index + 1, array_ + index, sizeof(T) * (size_ - index));
	array_[index] = element;
	size_++;
}

/// Removes an element at a specified position (shifting elements around)
template <class T>
void Array<T>::removeAt(unsigned int index)
{
	// Cannot remove past the last element
	if (index > size_ - 1)
	{
		LOGF_X("Element %u out of size range", index);
		exit(EXIT_FAILURE);
	}

	// memmove() takes care of overlapping regions
	memmove(array_ + index, array_ + index + 1, sizeof(T) * (size_ - index - 1));
	size_--;
}

/// Inserts an array of elements at the end in constant time
template <class T>
void Array<T>::append(const T* elements, unsigned int amount)
{
	if (size_ + amount > capacity_)
	{
		setCapacity(size_ + amount);
	}

	memcpy(array_ + size_, elements, sizeof(T)*amount);
	size_ += amount;
}

/// Read-only subscript operator
template <class T>
const T& Array<T>::operator[](const unsigned int index) const
{
	if (index > size_)
	{
		LOGF_X("Element %u out of size range", index);
		exit(EXIT_FAILURE);
	}

	return array_[index];
}

/// Subscript operator
template <class T>
T& Array<T>::operator[](const unsigned int index)
{
	// Avoid creating "holes" into the array
	if (index > size_)
	{
		LOGF_X("Element %u out of size range", index);
		exit(EXIT_FAILURE);
	}
	// Adding an element at the back of the array
	else if (index == size_)
	{
		// Need growing
		if (size_ == capacity_)
		{
			setCapacity(capacity_ * 2);
		}

		size_++;
	}

	return array_[index];
}

/// Allows for direct but unchecked access to the array memory
template <class T>
T* Array<T>::mapBuffer(unsigned int reserved)
{
	if (size_ + reserved > capacity_)
	{
		setCapacity(size_ + reserved);
	}

	T* array = &array_[size_];
	size_ += reserved;

	return array;
}

}

#endif
