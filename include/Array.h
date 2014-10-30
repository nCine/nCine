#ifndef CLASS_NCINE_ARRAY
#define CLASS_NCINE_ARRAY

#include <cstdio> // for NULL
#include <cstdlib> // for exit()
#include <cstring> // for memmove() and memcpy()
#include "ServiceLocator.h"

namespace ncine {

/// An array based on templates
template <class T>
class Array
{
  public:
	/// Constructs an array with explicit size
	explicit Array(unsigned int capacity)
		: array_(NULL), size_(0), capacity_(0)
	{
		setCapacity(capacity);
	}
	~Array() { delete[] array_; }

	/// Returns true if the array is empty
	inline bool isEmpty() const { return size_ == 0; }
	/// Returns the array size
	/** The array is filled without gaps until the Size()-1 element */
	inline unsigned int size() const { return size_; }
	/// Returns the array size
	/// The array has memory allocated to store untile the Capacity()-1 element
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
	// Inserts an array of elements at the end in constant time
	void append(const T* elements, unsigned int amount);
	/// Removes an element at a specified position (shifting elements around)
	void removeAt(unsigned int index);

	// Read-only subscript operator
	const T& operator[](const unsigned int index) const;
	// Subscript operator
	T& operator[](const unsigned int index);

	/// Returns a pointer to the allocated memory
	/** It's useful when holding arrays of OpenGL data */
	inline T* const pointer() const { return array_; }
	// Allows for direct but unchecked access to the array memory
	T* mapBuffer(unsigned int reserved);

  private:
	T* array_;
	unsigned int size_;
	unsigned int capacity_;

	// Preventing copy at the moment
	Array(const Array&);
	void operator=(const Array&);
};

/// Sets a new capacity for the array (can be bigger or smaller than the current one)
template <class T>
void Array<T>::setCapacity(unsigned int newCapacity)
{
	if (newCapacity == 0)
	{
		LOGF("Zero is not valid capacity");
		exit(EXIT_FAILURE);
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
	if (size_ + 1 > capacity_)
	{
		setCapacity(size_ * 2);
	}

	// memmove() takes care of overlapping regions
	memmove(array_ + index + 1, array_ + index, size_ - index);
	array_[index] = element;
	size_++;
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

template <class T>
void Array<T>::removeAt(unsigned int index)
{
	// memmove() takes care of overlapping regions
	memmove(array_ + index, array_ + index + 1, size_ - index);
	size_--;
}

/// Read-only subscript operator
template <class T>
const T& Array<T>::operator[](const unsigned int index) const
{
	if (index > size_)
	{
		LOGF_X("Element %u out of size range!", index);
		exit(EXIT_FAILURE);
	}

	return array_[index];
}

/// Subscript operator
template <class T>
T& Array<T>::operator[](const unsigned int index)
{
	if (index > capacity_ - 1)
	{
//		LOGW("Element %u out of capacity range!", index);
//		exit(EXIT_FAILURE);
	}

	// Avoid creating "holes" into the array
	if (index > size_)
	{
		LOGF_X("Element %u out of size range!", index);
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
