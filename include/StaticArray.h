#ifndef CLASS_NCINE_STATICARRAY
#define CLASS_NCINE_STATICARRAY

#include <cstdio> // for NULL
#include <cstdlib> // for exit()
#include "ArrayIterator.h"
#include "ServiceLocator.h"

namespace ncine {

/// A static array based on templates and stored on the stack
template <class T, unsigned int S>
class StaticArray
{
  public:
	/// Iterator type
	typedef ArrayIterator<T> Iterator;
	/// Constant iterator type
	typedef const ArrayIterator<T> Const_Iterator;

	/// Constructs an array with fixed size
	explicit StaticArray() : size_(S) { }

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

	/// Returns the array size
	inline unsigned int size() const { return size_; }

	// Read-only subscript operator
	const T& operator[](unsigned int index) const;
	// Subscript operator
	T& operator[](unsigned int index);

	/// Returns a pointer to the allocated memory
	inline T* data() { return array_; }
	/// Returns a constant pointer to the allocated memory
	inline const T* data() const { return array_; }

  private:
	T array_[S];
	unsigned int size_;
};

/// Copy constructor
template <class T, unsigned int S>
StaticArray<T, S>::StaticArray(const StaticArray<T, S>& other)
	: size_(other.size_)
{
	for (unsigned int i = 0; i < other.size_; i++)
	{
		array_[i] = other.array_[i];
	}
}

/// Assignment operator
template <class T, unsigned int S>
StaticArray<T, S>& StaticArray<T, S>::operator=(const StaticArray<T, S>& other)
{
	for (unsigned int i = 0; i < other.size_; i++)
	{
		array_[i] = other.array_[i];
	}

	return *this;
}

/// Read-only subscript operator
template <class T, unsigned int S>
const T& StaticArray<T, S>::operator[](const unsigned int index) const
{
	if (index >= size_)
	{
		LOGF_X("Element %u out of size range", index);
		exit(EXIT_FAILURE);
	}

	return array_[index];
}

/// Subscript operator
template <class T, unsigned int S>
T& StaticArray<T, S>::operator[](const unsigned int index)
{
	if (index >= size_)
	{
		LOGF_X("Element %u out of size range", index);
		exit(EXIT_FAILURE);
	}

	return array_[index];
}

}

#endif
