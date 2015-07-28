#ifndef CLASS_NCINE_STATICARRAY
#define CLASS_NCINE_STATICARRAY

#include <cstdio> // for NULL
#include <cstdlib> // for exit()
#include "ArrayIterator.h"
#include "ServiceLocator.h"

namespace ncine {

/// A static array based on templates and stored on the stack
template <class T, unsigned int C>
class StaticArray
{
  public:
	/// Iterator type
	typedef ArrayIterator<T> Iterator;
	/// Constant iterator type
	typedef const ArrayIterator<T> Const_Iterator;

	/// Constructs an empty array with fixed capacity
	explicit StaticArray() : size_(0), capacity_(C) { }
	/// Constructs an array the option for it to have the size as its capacity
	StaticArray(bool extend)
		: size_(0), capacity_(C)
	{
		if (extend)
		{
			size_ = capacity_;
		}
	}

	// Copy constructor
	StaticArray(const StaticArray& other);
	// Assignment operator
	StaticArray& operator=(const StaticArray& other);

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
	inline unsigned int capacity() const { return capacity_; }
	// Sets a new size for the array (allowing for "holes")
	void setSize(unsigned int newSize);

	/// Clears the array
	/** Size will be set to zero but capacity remains untouched */
	inline void clear() { size_ = 0; }
	/// Inserts a new element as the last one in constant time
	inline void insertBack(T element) { operator[](size_) = element; }

	// Read-only subscript operator
	const T& operator[](unsigned int index) const;
	// Subscript operator
	T& operator[](unsigned int index);

	/// Returns a constant pointer to the allocated memory
	inline const T* data() const { return array_; }
	/// Returns a pointer to the allocated memory
	inline T* data() { return array_; }

  private:
	T array_[C];
	unsigned int size_;
	unsigned int capacity_;
};

/// Copy constructor
template <class T, unsigned int C>
StaticArray<T, C>::StaticArray(const StaticArray<T, C>& other)
	: size_(other.size_), capacity_(other.capacity_)
{
	for (unsigned int i = 0; i < other.size_; i++)
	{
		// copying all elements invoking their copy constructor
		array_[i] = other.array_[i];
	}
}

/// Assignment operator
template <class T, unsigned int C>
StaticArray<T, C>& StaticArray<T, C>::operator=(const StaticArray<T, C>& other)
{
	size_ = other.size_;
	for (unsigned int i = 0; i < other.size_; i++)
	{
		// copying all elements invoking their assignment operator
		array_[i] = other.array_[i];
	}

	return *this;
}

/// Sets a new size for the array (allowing for "holes")
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

/// Read-only subscript operator
template <class T, unsigned int C>
const T& StaticArray<T, C>::operator[](unsigned int index) const
{
	if (index >= size_)
	{
		LOGF_X("Element %u out of size range", index);
		exit(EXIT_FAILURE);
	}

	return array_[index];
}

/// Subscript operator
template <class T, unsigned int C>
T& StaticArray<T, C>::operator[](unsigned int index)
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
			LOGW_X("Element %u out of capacity range", index);
		}
		else
		{
			size_++;
		}
	}

	return array_[index];
}

}

#endif
